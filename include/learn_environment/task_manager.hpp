#ifndef TASKMANAGER_HPP
#define TASKMANAGER_HPP

#include "task.hpp"
#include "task_executor.hpp"
#include "execute_frame.hpp"

#include <QObject>
#include <QPushButton>
#include <QFrame>
#include <QVector>
#include <QSharedPointer>

class TaskUI;

/**
 * @class TaskManager
 * @brief Manages tasks and their execution within the application.
 *
 * The TaskManager class is responsible for loading tasks, managing their execution,
 * and handling user interactions through the UI.
 */
class TaskManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a TaskManager object.
     * @param taskUI Pointer to the TaskUI object.
     * @param nextButton Pointer to the next button.
     * @param previousButton Pointer to the previous button.
     * @param parent Pointer to the parent QObject.
     */
    TaskManager(TaskUI *taskUI, QPushButton *nextButton, QPushButton *previousButton, QFrame *resetRobotFrame, QObject *parent = nullptr);

    /**
     * @brief Starts or stops a subtask.
     * @param subtask Reference to the subtask to be started or stopped.
     * @param startSolution Flag indicating whether to start the solution or the users script.
     */
    void startStopSubtask(Subtask &subtask, bool startSolution = false);

    /**
     * @brief Show or hide the solution of a subtask.
     * @param subtask Reference to the subtask.
     */
    void toggleSolution(Subtask &subtask);

public Q_SLOTS:
    /**
     * @brief Starts or stops a subtask.
     * @param subtask Reference to the subtask to be started or stopped.
     */
    void startStopSubtask(const Subtask &subtask);

    /**
     * @brief Forces the reset of the robot to its initial state.
     */
    void forceResetRobot();

private Q_SLOTS:
    /**
     * @brief Slot for handling the next button click event.
     */
    void onNextButtonClicked();

    /**
     * @brief Slot for handling the previous button click event.
     */
    void onPreviousButtonClicked();

    /**
     * @brief Selects a task by its index.
     * @param index The index of the task to be selected.
     */
    void selectTask(int index);

    /**
     * @brief Slot for handling the task execution started event.
     */
    void onTaskExecutionStarted();

    /**
     * @brief Slot for handling the task execution finished event.
     */
    void onTaskExecutionFinished();

    /**
     * @brief Slot for handling the task execution failed event.
     * @param error The error message.
     */
    void onTaskExecutionFailed(const QString &error);

    /**
     * @brief Slot for handling the reset robot started event.
     */
    void onResetRobotStarted();

    /**
     * @brief Slot for handling the reset robot finished event.
     */
    void onResetRobotFinished();

    /**
     * @brief Slot for handling the reset robot failed event.
     * @param error The error message.
     */
    void onResetRobotFailed(const QString &error);

private:
    TaskUI *taskUI; ///< Pointer to the TaskUI object.
    TaskExecutor *taskExecutor; ///< Pointer to the TaskExecutor object.
    QPushButton *nextButton; ///< Pointer to the next button.
    QPushButton *previousButton; ///< Pointer to the previous button.
    QFrame *resetRobotFrame; ///< Pointer to the reset robot frame.
    ExecuteFrame *executeResetRobotFrame; ///< Pointer to the execute reset robot frame.
    QVector<QSharedPointer<Task>> tasks; ///< Vector of tasks.
    QVector<Subtask*> queued_and_running_subtasks; ///< Vector of queued and running subtasks.
    int currentQueueStartSolution = false; ///< Flag indicating whether to start the solution or the users script.
    int currentTaskIndex; ///< Index of the current task.
    bool resetRobotInProgress = false; ///< Flag indicating whether the robot reset is in progress.

    /**
     * @brief Starts a subtask.
     * @param started_subtask Reference to the subtask to be started.
     * @param task Shared pointer to the task containing the subtask.
     */
    void startSubtask(Subtask &started_subtask, QSharedPointer<Task> &task, bool startSolution = false);

    /**
     * @brief Starts the first subtask and logs a message.
     */
    void initiateFirstSubtask();

    /**
     * @brief Forces the stop of the current task execution.
     */
    void forceStop();

    /**
     * @brief Logs a message surrounded by hashes.
     * @param message The message to be logged.
     */
    void logWithHashes(const QString &message);
};

#endif // TASKMANAGER_HPP