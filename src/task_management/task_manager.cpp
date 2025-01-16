#include "task_management/task_manager.hpp"
#include "ui/task_ui.hpp"
#include "task_management/task_parser.hpp"
#include "script_execution/task_executor.hpp"
#include "common/notebook_converter.hpp"
#include "common/folder_structure_constants.hpp"

#include <QDebug>
#include <QToolButton>

TaskManager::TaskManager(TaskUI *taskUI, QObject *parent)
    : QObject(parent),
      taskUI(taskUI),
      taskExecutor(new TaskExecutor(this)),
      currentTaskIndex(0)
{
    TaskParser parser;
    tasks = parser.loadTasks(FolderStructureConstants::TASK_DEFINITIONS_PATH, 
                             FolderStructureConstants::DIFFICULTY_LEVELS_DEFINITION_PATH, 
                             FolderStructureConstants::TOPIC_DEFINITIONS_PATH);

    if (tasks.isEmpty()) {
        qCritical() << "No tasks loaded. Exiting TaskManager initialization.";
        return;
    }

    // Connect TaskExecutor signals
    connect(taskExecutor, &TaskExecutor::taskExecutionStarted, this, &TaskManager::onTaskExecutionStarted);
    connect(taskExecutor, &TaskExecutor::taskExecutionFinished, this, &TaskManager::onTaskExecutionFinished);
    connect(taskExecutor, &TaskExecutor::taskExecutionFailed, this, &TaskManager::onTaskExecutionFailed);

    connect(taskExecutor, &TaskExecutor::resetRobotStarted, this, &TaskManager::onResetRobotStarted);
    connect(taskExecutor, &TaskExecutor::resetRobotFinished, this, &TaskManager::onResetRobotFinished);
    connect(taskExecutor, &TaskExecutor::resetRobotFailed, this, &TaskManager::onResetRobotFailed);

    taskUI->setTaskManager(this);
    taskUI->initializeUI(tasks);

    selectTask(currentTaskIndex);
}

void TaskManager::startStopSubtask(Subtask &subtask, bool startSolution)
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
            startSubtask(subtask, currentTask, startSolution);
        } else if (subtask.status == SubtaskStatus::Running) {
            forceStop();
        }
    } else {
        qCritical() << "Parent task is no longer available.";
    }
}

void TaskManager::toggleSolution(Subtask &subtask)
{
    QSharedPointer<Task> parentTaskPtr = subtask.parentTask.lock();
    if (!parentTaskPtr) {
        qCritical() << "Parent task is no longer available.";
        return;
    }

    NotebookConverter converter;
    converter.toggleSolution(subtask.filePath, subtask.solutionFilePath);

    taskUI->updateSubtaskItemsUI();
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

void TaskManager::nextTask()
{
    if (currentTaskIndex < tasks.size() - 1) {
        selectTask(currentTaskIndex + 1);
    }
}

void TaskManager::previousTask()
{
    if (currentTaskIndex > 0) {
        selectTask(currentTaskIndex - 1);
    }
}

void TaskManager::startSubtask(Subtask &started_subtask, QSharedPointer<Task> &task, bool startSolution)
{
    // Invalidate all subtasks
    for (auto &subtask : task->subtasks) {
        subtask.status = SubtaskStatus::Inactive;
    }

    // Add started subtask and previous subtasks if required to execution queue
    if (task->previousSubtasksRequired) {
        for (auto &subtask : task->subtasks) {
            queued_and_running_subtasks.append(&subtask);
            subtask.status = SubtaskStatus::Queued;
            if (&subtask == &started_subtask) {
                break;
            }
        }
    } else {
        queued_and_running_subtasks.append(&started_subtask);
        started_subtask.status = SubtaskStatus::Queued;
    }

    // reset Robot and start subtasks
    if (!queued_and_running_subtasks.isEmpty()) {
        for (auto &subtask : queued_and_running_subtasks) {
            subtask->hasBeenExecuted = true;
        }
        
        currentQueueStartSolution = startSolution;

        if (started_subtask.reset_robot_before_executing) {
            qDebug() << "Resetting the robot before executing the subtask.";
            taskExecutor->resetRobot();
        } else {
            qDebug() << "Robot reset not required for this subtask.";
            initiateFirstSubtask();
        }
    } else {
        qCritical() << "No subtask queued.";
    }
     
    taskUI->updateSubtaskItemsUI();
}

void TaskManager::forceResetRobot()
{
    if (resetRobotInProgress) {
        qWarning() << "Robot reset already in progress.";
        forceStop();
        return;
    }
    qDebug() << "Forcing reset of the robot.";
    forceStop();
    taskExecutor->resetRobot();
}

void TaskManager::forceStop()
{
    taskExecutor->forceStop();
    if (!queued_and_running_subtasks.isEmpty()) {
        for (auto &subtask : queued_and_running_subtasks) {
            subtask->lastExecutionFailed = true;
            subtask->lastExecutionError = "<b>Execution stopped by user.</b>";
        }
        queued_and_running_subtasks.clear();
        for (auto& task : tasks) {
            for (Subtask& sub : task->subtasks) {
                sub.status = SubtaskStatus::Ready;
            }
        }
        taskUI->updateSubtaskItemsUI();
    }
    currentQueueStartSolution = false;
}

void TaskManager::initiateFirstSubtask()
{
    if (!queued_and_running_subtasks.isEmpty()) {        
        // start subtasks
        Subtask *first_subtask = queued_and_running_subtasks.first();
        first_subtask->status = SubtaskStatus::Running;
        taskExecutor->executeTask(*first_subtask, currentQueueStartSolution);
    }
    taskUI->updateSubtaskItemsUI();
}

void TaskManager::onTaskExecutionStarted()
{
    if (queued_and_running_subtasks.isEmpty()) { 
        return;
    }
    for (int i = 0; i < queued_and_running_subtasks.size(); ++i) {
        queued_and_running_subtasks[i]->lastExecutionError = "";
        queued_and_running_subtasks[i]->lastExecutionFailed = false;
    }
    Subtask *first_subtask = queued_and_running_subtasks.first();
    QString title = first_subtask->title;
    QString message = currentQueueStartSolution 
        ? QString("Solution for subtask \"%1\" started").arg(title)
        : QString("Subtask \"%1\" started").arg(title);
    logWithHashes(message);
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
            currentQueueStartSolution = false;
        } else {
            qDebug() << "Task execution finished: " << first_subtask->title << " - Initiating next subtask.";
                
            Subtask *next_subtask = queued_and_running_subtasks.first();
            next_subtask->status = SubtaskStatus::Running;
            taskExecutor->executeTask(*next_subtask, currentQueueStartSolution);
            taskUI->updateSubtaskItemsUI();
        }
    } else {
        qCritical() << "Parent task is no longer available.";
    }
}

void TaskManager::onTaskExecutionFailed(const QString &error)
{
    if (queued_and_running_subtasks.isEmpty()) { 
        return;
    }

    Subtask *first_subtask = queued_and_running_subtasks.first();

    first_subtask->lastExecutionError = error;
    first_subtask->lastExecutionFailed = true;

    for (int i = 1; i < queued_and_running_subtasks.size(); ++i) {
        queued_and_running_subtasks[i]->lastExecutionError = "<b>This subtask was not executed due to failure of the previous subtask.</b>";
        queued_and_running_subtasks[i]->lastExecutionFailed = true;
    }

    if (auto parentTask = first_subtask->parentTask.lock()) {
        queued_and_running_subtasks.clear();
        for (auto &subtask : parentTask->subtasks) {
            subtask.status = SubtaskStatus::Ready;
        }
        taskUI->updateSubtaskItemsUI();
        currentQueueStartSolution = false;
    } else {
        qCritical() << "Parent task is no longer available.";
    }
}

void TaskManager::onResetRobotStarted()
{
    if (resetRobotInProgress) {
        qWarning() << "Robot reset already in progress.";
        forceStop();
        return;
    }

    resetRobotInProgress = true;

    bool noSubtasksLeft = queued_and_running_subtasks.isEmpty();
    taskUI->startedRobotResetUI(noSubtasksLeft);

    logWithHashes("Robot reset started.");
}

void TaskManager::onResetRobotFinished()
{
    resetRobotInProgress = false;
    
    taskUI->finishedRobotResetUI();

    initiateFirstSubtask();
}
    
void TaskManager::onResetRobotFailed(const QString &error)
{
    qCritical() << "Robot reset failed:" << error;

    resetRobotInProgress = false;

    taskUI->failedRobotResetUI(error);

    onTaskExecutionFailed(error);
}

void TaskManager::logWithHashes(const QString &message)
{
    int hashCount = message.length();
    QString hashes(hashCount + 2, '#');
    QString initial_hashes(4, '#');
    qDebug().noquote().nospace()
        << initial_hashes << hashes << initial_hashes << "\n"
        << initial_hashes << " " << message << " " << initial_hashes << "\n"
        << initial_hashes << hashes << initial_hashes;
}