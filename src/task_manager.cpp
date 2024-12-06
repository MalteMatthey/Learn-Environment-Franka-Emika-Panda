#include "learn_environment/task_manager.h"
#include "learn_environment/task_ui.h"
#include "learn_environment/task_parser.h"
#include "learn_environment/task_executor.h"
#include <QDebug>

TaskManager::TaskManager(TaskUI *taskUI, QPushButton *nextButton, QPushButton *previousButton, QObject *parent)
    : QObject(parent),
      taskUI(taskUI),
      taskExecutor(new TaskExecutor(this)),
      nextButton(nextButton),
      previousButton(previousButton),
      currentTaskIndex(0)
{
    TaskParser parser;
    tasks = parser.loadTasks(":/tasks/task_definitions.json");

    if (tasks.isEmpty()) {
        qCritical() << "No tasks loaded. Exiting TaskManager initialization.";
        return;
    }

    // Connect TaskExecutor signals
    connect(taskExecutor, &TaskExecutor::taskExecutionStarted, this, &TaskManager::onTaskExecutionStarted);
    connect(taskExecutor, &TaskExecutor::taskExecutionFinished, this, &TaskManager::onTaskExecutionFinished);
    connect(taskExecutor, &TaskExecutor::taskExecutionFailed, this, &TaskManager::onTaskExecutionFailed);

    taskUI->initializeUI(tasks);

    selectTask(currentTaskIndex);

    connect(nextButton, &QPushButton::clicked, this, &TaskManager::onNextButtonClicked);
    connect(previousButton, &QPushButton::clicked, this, &TaskManager::onPreviousButtonClicked);

    // Connect TaskUI's taskSelected signal to the selectTask slot
    connect(taskUI, &TaskUI::taskSelected, this, &TaskManager::selectTask);
}

void TaskManager::startStopSubtask(Subtask &subtask)
{
    qDebug() << "Start/Stop subtask in TaskManager requested:" << subtask.title;
    if (subtask.status == SubtaskStatus::Inactive) {
        qCritical() << "Inactive subtask cannot be started";
        return;
    }

    // debug current status in switch case statement
    switch (subtask.status) {
        case SubtaskStatus::Ready:
            qDebug() << "Subtask is ready";
            break;
        case SubtaskStatus::Running:
            qDebug() << "Subtask is running";
            break;
        case SubtaskStatus::Queued:
            qDebug() << "Subtask is queued";
            break;
        case SubtaskStatus::Inactive:
            qDebug() << "Subtask is inactive";
            break;
    }


    if (subtask.status == SubtaskStatus::Ready) {
        if (currentTaskIndex < 0 || currentTaskIndex >= tasks.size()) {
            qCritical() << "Invalid currentTaskIndex in startStopSubtask:" << currentTaskIndex;
            return;
        }

        QSharedPointer<Task> currentTask = tasks[currentTaskIndex];
        if (subtask.parentTask != currentTask) {
            qCritical() << "Subtask to be executed does not belong to the current task";
            return;
        }

        taskExecutor->executeTask(subtask);
        subtask.status = SubtaskStatus::Running;
        taskUI->updateSubtaskItemsUI();
    } else if (subtask.status == SubtaskStatus::Running) {
        taskExecutor->forceStop();
        subtask.status = SubtaskStatus::Ready;
        taskUI->updateSubtaskItemsUI();
    }

}

void TaskManager::selectTask(int index)
{
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

void TaskManager::onTaskExecutionStarted()
{
    qDebug() << "Task execution started";
}

void TaskManager::onTaskExecutionFinished()
{
    qDebug() << "Task execution finished";
}

void TaskManager::onTaskExecutionFailed(const QString &error)
{
    qDebug() << "Task execution failed:" << error;
}