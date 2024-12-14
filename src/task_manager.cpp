#include "learn_environment/task_manager.hpp"
#include "learn_environment/task_ui.hpp"
#include "learn_environment/task_parser.hpp"
#include "learn_environment/task_executor.hpp"
#include "learn_environment/notebook_converter.hpp"

#include <QDebug>
#include <QToolButton>

namespace {
    const char* TASK_DEFINITIONS_PATH = ":/task_pool/task_definitions.json";
    const char* TOPIC_DEFINITIONS_PATH = ":/task_pool/topic_definitions.json";
    const char* DIFFICULTY_LEVELS_DEFINITION_PATH = ":/task_pool/difficulty_levels.json";
    const char* LOADING_GIF_PATH = ":/resource/icons/loading.gif";
    const char* START_ICON_PATH = ":/resource/icons/play.png";
    const char* STOP_ICON_PATH = ":/resource/icons/stop.png";
    const char* FAILED_ICON_PATH = ":/resource/icons/failed.png";
    const char* SUCCEEDED_ICON_PATH = ":/resource/icons/succeeded.png";
    const char* RESET_ROBOT_TEXT = "Resetting the robot to its default state...";
    const char* RESET_ROBOT_TEXT_SCRIPTS = "Resetting the robot before script execution...";
    const char* RESET_ROBOT_TEXT_SUCCESS = "Robot successfully reset!";
    const char* RESET_ROBOT_TEXT_FAILED = "<b>Robot reset failed!</b>";
    const char* STOP_RESET_ROBOT_TOOLTIP = "Cancel reset process";
    const char* RESET_ROBOT_TOOLTIP = "Reset robot to default state";
    const char* RESET_ROBOT_START_BUTTON_NAME = "resetRobotStartButton";
}

TaskManager::TaskManager(TaskUI *taskUI, QPushButton *nextButton, QPushButton *previousButton, QFrame *resetRobotFrame, QObject *parent)
    : QObject(parent),
      taskUI(taskUI),
      taskExecutor(new TaskExecutor(this)),
      nextButton(nextButton),
      previousButton(previousButton),
      resetRobotFrame(resetRobotFrame),
      executeResetRobotFrame(nullptr),
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

    connect(taskExecutor, &TaskExecutor::resetRobotStarted, this, &TaskManager::onResetRobotStarted);
    connect(taskExecutor, &TaskExecutor::resetRobotFinished, this, &TaskManager::onResetRobotFinished);
    connect(taskExecutor, &TaskExecutor::resetRobotFailed, this, &TaskManager::onResetRobotFailed);

    taskUI->setTaskManager(this);
    taskUI->initializeUI(tasks);

    selectTask(currentTaskIndex);

    connect(nextButton, &QPushButton::clicked, this, &TaskManager::onNextButtonClicked);
    connect(previousButton, &QPushButton::clicked, this, &TaskManager::onPreviousButtonClicked);

    // Connect TaskUI's taskSelected signal to the selectTask slot
    connect(taskUI, &TaskUI::taskSelected, this, &TaskManager::selectTask);
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

    QString message = "Robot reset started.";
    resetRobotInProgress = true;
    
    QToolButton* resetRobotStartButton = resetRobotFrame->findChild<QToolButton*>(RESET_ROBOT_START_BUTTON_NAME);
    if (resetRobotStartButton) {
        resetRobotStartButton->setIcon(QIcon(STOP_ICON_PATH));
        resetRobotStartButton->setToolTip(STOP_RESET_ROBOT_TOOLTIP);
    } else {
        qCritical() << "Reset robot start button not found.";
    }

    if (!executeResetRobotFrame) {
        executeResetRobotFrame = new ExecuteFrame(resetRobotFrame);
        QVBoxLayout *resetLayout = qobject_cast<QVBoxLayout*>(resetRobotFrame->layout());
        if (!resetLayout) {
            resetLayout = new QVBoxLayout(resetRobotFrame);
            resetRobotFrame->setLayout(resetLayout);
        }
        if (!resetLayout) {
            qCritical() << "Reset layout not found.";
            return;
        }
        resetLayout->addWidget(executeResetRobotFrame);
    }
    if (!executeResetRobotFrame) {
        qCritical() << "Execute reset robot frame not found.";
        return;
    }
    executeResetRobotFrame->setImage(LOADING_GIF_PATH);
    if (queued_and_running_subtasks.isEmpty()) {
        executeResetRobotFrame->setText(RESET_ROBOT_TEXT);
    } else {
        executeResetRobotFrame->setText(RESET_ROBOT_TEXT_SCRIPTS);
    }
    logWithHashes(message);
}

void TaskManager::onResetRobotFinished()
{
    if (resetRobotInProgress) {
        logWithHashes("Robot reset finished.");
    }

    resetRobotInProgress = false;
    // update UI
    QToolButton* resetRobotStartButton = resetRobotFrame->findChild<QToolButton*>(RESET_ROBOT_START_BUTTON_NAME);
    if (resetRobotStartButton) {
        resetRobotStartButton->setIcon(QIcon(START_ICON_PATH));
        resetRobotStartButton->setToolTip(RESET_ROBOT_TOOLTIP);
    }
    if (executeResetRobotFrame) {
        if (!executeResetRobotFrame->getText().contains(RESET_ROBOT_TEXT_FAILED)) {
            executeResetRobotFrame->setImage(SUCCEEDED_ICON_PATH);
            executeResetRobotFrame->setText(RESET_ROBOT_TEXT_SUCCESS);
        }
    }

    initiateFirstSubtask();
}
    
void TaskManager::onResetRobotFailed(const QString &error)
{
    qCritical() << "Robot reset failed:" << error;

    resetRobotInProgress = false;

    if (executeResetRobotFrame) {
        executeResetRobotFrame->setImage(FAILED_ICON_PATH);
        executeResetRobotFrame->setText(QString(RESET_ROBOT_TEXT_FAILED) + "<br>" + error);
    }

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