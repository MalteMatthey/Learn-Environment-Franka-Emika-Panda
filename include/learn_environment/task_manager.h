#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include "task.h"
#include "task_executor.h"

#include <QObject>
#include <QPushButton>
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
    TaskManager(TaskUI *taskUI, QPushButton *nextButton, QPushButton *previousButton, QObject *parent = nullptr);

    /**
     * @brief Starts or stops a subtask.
     * @param subtask Reference to the subtask to be started or stopped.
     */
    void startStopSubtask(Subtask &subtask);

public Q_SLOTS:
    /**
     * @brief Starts or stops a subtask.
     * @param subtask Reference to the subtask to be started or stopped.
     */
    void startStopSubtask(const Subtask &subtask);

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

private:
    TaskUI *taskUI; ///< Pointer to the TaskUI object.
    TaskExecutor *taskExecutor; ///< Pointer to the TaskExecutor object.
    QPushButton *nextButton; ///< Pointer to the next button.
    QPushButton *previousButton; ///< Pointer to the previous button.
    QVector<QSharedPointer<Task>> tasks; ///< Vector of tasks.
    QVector<Subtask*> queued_and_running_subtasks; ///< Vector of queued and running subtasks.
    int currentTaskIndex; ///< Index of the current task.

    /**
     * @brief Starts a subtask.
     * @param started_subtask Reference to the subtask to be started.
     * @param task Shared pointer to the task containing the subtask.
     */
    void startSubtask(Subtask &started_subtask, QSharedPointer<Task> &task);

    /**
     * @brief Forces the stop of the current task execution.
     */
    void forceStop();
};

#endif // TASKMANAGER_H