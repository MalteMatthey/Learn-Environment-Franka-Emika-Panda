#include "learn_environment/task_manager.hpp"
#include "learn_environment/task_ui.hpp"
#include "learn_environment/task_parser.hpp"
#include "learn_environment/task_executor.hpp"
#include "learn_environment/notebook_converter.hpp"

#include <QDebug>

namespace {
    const char* TASK_DEFINITIONS_PATH = ":/task_pool/task_definitions.json";
    const char* TOPIC_DEFINITIONS_PATH = ":/task_pool/topic_definitions.json";
    const char* DIFFICULTY_LEVELS_DEFINITION_PATH = ":/task_pool/difficulty_levels.json";
}

TaskManager::TaskManager(TaskUI *taskUI, QPushButton *nextButton, QPushButton *previousButton, QObject *parent)
    : QObject(parent),
      taskUI(taskUI),
      taskExecutor(new TaskExecutor(this)),
      nextButton(nextButton),
      previousButton(previousButton),
      currentTaskIndex(0)
{
    TaskParser parser;
    tasks = parser.loadTasks(TASK_DEFINITIONS_PATH, DIFFICULTY_LEVELS_DEFINITION_PATH, TOPIC_DEFINITIONS_PATH);

    if (tasks.isEmpty()) {
        qCritical() << "No tasks loaded. Exiting TaskManager initialization.";
        return;
    }

    // Connect TaskExecutor signals
    connect(taskExecutor, &TaskExecutor::taskExecutionStarted, this, &TaskManager::onTaskExecutionStarted);
    connect(taskExecutor, &TaskExecutor::taskExecutionFinished, this, &TaskManager::onTaskExecutionFinished);
    connect(taskExecutor, &TaskExecutor::taskExecutionFailed, this, &TaskManager::onTaskExecutionFailed);

    taskUI->setTaskManager(this);
    taskUI->initializeUI(tasks);

    selectTask(currentTaskIndex);

    connect(nextButton, &QPushButton::clicked, this, &TaskManager::onNextButtonClicked);
    connect(previousButton, &QPushButton::clicked, this, &TaskManager::onPreviousButtonClicked);

    // Connect TaskUI's taskSelected signal to the selectTask slot
    connect(taskUI, &TaskUI::taskSelected, this, &TaskManager::selectTask);
}

void TaskManager::startStopSubtask(Subtask &subtask)
{
    if (auto parentTask = subtask.parentTask.lock()) {
        if (currentTaskIndex < 0 || currentTaskIndex >= tasks.size()) {
            qCritical() << "Invalid currentTaskIndex in startStopSubtask:" << currentTaskIndex;
            return;
        }

        QSharedPointer<Task> currentTask = tasks[currentTaskIndex];
        if (parentTask != currentTask) {
            qCritical() << "Subtask to be executed does not belong to the current task";
            return;
        }

        if (subtask.status == SubtaskStatus::Inactive) {
            qCritical() << "Inactive subtask cannot be started";
            return;
        }

        if (subtask.status == SubtaskStatus::Ready) {
            startSubtask(subtask, currentTask);
        } else if (subtask.status == SubtaskStatus::Running) {
            forceStop();
        }
    } else {
        qCritical() << "Parent task is no longer available.";
    }
}

void TaskManager::toggleSolution(Subtask &subtask)
{
    qDebug() << "Toggling solution for subtask:" << subtask.title;
    QSharedPointer<Task> parentTaskPtr = subtask.parentTask.lock();
    if (!parentTaskPtr) {
        qCritical() << "Parent task is no longer available.";
        return;
    }

    NotebookConverter converter;
    converter.toggleSolution(parentTaskPtr->folder + subtask.file, subtask.solutionFilePath);
}

void TaskManager::selectTask(int index)
{
    // check if task is currently getting executed
    if (!queued_and_running_subtasks.isEmpty()) {
        forceStop();
    }

    if (index >= 0 && index < static_cast<int>(tasks.size())) {
        currentTaskIndex = index;
        taskUI->setTaskUI(currentTaskIndex);

        // Set all subtasks status to inactive except the currentTaskIndex task
        for (int i = 0; i < tasks.size(); ++i) {
            QSharedPointer<Task> task = tasks[i];
            for (Subtask &subtask : task->subtasks) {
                subtask.status = (i == index) ? SubtaskStatus::Ready : SubtaskStatus::Inactive;
            }
        }

        taskUI->updateSubtaskItemsUI();
    } else {
        qWarning() << "Invalid task index selected:" << index;
    }
}

void TaskManager::onNextButtonClicked()
{
    if (currentTaskIndex < tasks.size() - 1) {
        selectTask(currentTaskIndex + 1);
    }
}

void TaskManager::onPreviousButtonClicked()
{
    if (currentTaskIndex > 0) {
        selectTask(currentTaskIndex - 1);
    }
}

void TaskManager::startSubtask(Subtask &started_subtask, QSharedPointer<Task> &task)
{
    // Invalidate all subtasks
    for (auto &subtask : task->subtasks) {
        subtask.status = SubtaskStatus::Inactive;
    }

    // Add started subtask and previous subtasks if required to execution queue
    if (task->previousSubtasksRequired) {
        for (auto &subtask : task->subtasks) {
            queued_and_running_subtasks.append(&subtask); // Store pointer
            subtask.status = SubtaskStatus::Queued;
            if (&subtask == &started_subtask) {
                break;
            }
        }
    } else {
        queued_and_running_subtasks.append(&started_subtask); // Store pointer
        started_subtask.status = SubtaskStatus::Queued;
    }

    // Execute first subtask
    if (!queued_and_running_subtasks.isEmpty()) {
        Subtask *first_subtask = queued_and_running_subtasks.first();
        first_subtask->status = SubtaskStatus::Running;
        taskExecutor->executeTask(*first_subtask);
    } else {
        qCritical() << "No subtask queued.";
    }
     
    taskUI->updateSubtaskItemsUI();
}

void TaskManager::forceStop()
{
    if (!queued_and_running_subtasks.isEmpty()) {
        taskExecutor->forceStop();
        queued_and_running_subtasks.clear();
        for (auto& task : tasks) {
            for (Subtask& sub : task->subtasks) {
                sub.status = SubtaskStatus::Ready;
            }
        }
        taskUI->updateSubtaskItemsUI();
    }
}

void TaskManager::onTaskExecutionStarted()
{
    qDebug() << "Task execution started";
}

void TaskManager::onTaskExecutionFinished()
{
    if (queued_and_running_subtasks.isEmpty()) { 
        return;
    }

    Subtask *first_subtask = queued_and_running_subtasks.first();
    first_subtask->status = SubtaskStatus::Inactive;

    if (auto parentTask = first_subtask->parentTask.lock()) {

        queued_and_running_subtasks.removeFirst();
        if (queued_and_running_subtasks.isEmpty()) { 
            qDebug() << "Task execution finished: " << first_subtask->title;

            for (auto &subtask : parentTask->subtasks) {
                subtask.status = SubtaskStatus::Ready;
            }
            taskUI->updateSubtaskItemsUI();
        } else {
            qDebug() << "Task execution finished: " << first_subtask->title << " - Initiating next subtask.";
                
            Subtask *next_subtask = queued_and_running_subtasks.first();
            next_subtask->status = SubtaskStatus::Running;
            taskExecutor->executeTask(*next_subtask);
            taskUI->updateSubtaskItemsUI();
        }
    } else {
        qCritical() << "Parent task is no longer available.";
    }
}

void TaskManager::onTaskExecutionFailed(const QString &error)
{
    qDebug() << "Task execution failed:" << error;

    if (queued_and_running_subtasks.isEmpty()) { 
        return;
    }

    Subtask *first_subtask = queued_and_running_subtasks.first();

    if (auto parentTask = first_subtask->parentTask.lock()) {
        queued_and_running_subtasks.clear();
        for (auto &subtask : parentTask->subtasks) {
            subtask.status = SubtaskStatus::Ready;
        }
        taskUI->updateSubtaskItemsUI();
    } else {
        qCritical() << "Parent task is no longer available.";
    }
}
