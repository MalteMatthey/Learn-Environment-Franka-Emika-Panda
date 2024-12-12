#ifndef TASK_EXECUTOR_HPP
#define TASK_EXECUTOR_HPP

#include "task.hpp"

#include <QObject>
#include <QString>
#include <QSharedPointer>

class ScriptWorker;

/**
 * @class TaskExecutor
 * @brief Executes tasks and manages their execution state.
 *
 * The TaskExecutor class is responsible for executing tasks and managing their execution state.
 * It interacts with ScriptWorker objects to perform the actual execution of tasks.
 */
class TaskExecutor : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a TaskExecutor object.
     * @param parent Pointer to the parent QObject.
     */
    explicit TaskExecutor(QObject *parent = nullptr);

    /**
     * @brief Executes a given subtask.
     * @param subtask The subtask to be executed.
     */
    void executeTask(const Subtask &subtask);

    /**
     * @brief Forces the stop of all currently running tasks.
     */
    void forceStop();

    /**
     * @brief Triggers python script to reset the robot to its initial state and removes all objects from the scene.
     */
    void resetRobot();

Q_SIGNALS:
    /**
     * @brief Signal emitted when task execution starts.
     */
    void taskExecutionStarted();

    /**
     * @brief Signal emitted when task execution finishes.
     */
    void taskExecutionFinished();

    /**
     * @brief Signal emitted when task execution fails.
     * @param error The error message.
     */
    void taskExecutionFailed(const QString &error);

    /**
     * @brief Signal emitted when robot reset finishes.
     */
    void resetRobotFinished();

    /**
     * @brief Signal emitted when robot reset fails.
     * @param error The error message.
     */
    void resetRobotFailed(const QString &error);

private:
    /**
     * @brief Constructs a file path from a base path and an addition.
     * @param basePath The base path.
     * @param addition The additional path component.
     * @param result The resulting constructed path.
     * @param errorMsg The error message to be used if construction fails.
     * @param checkExists Whether to check if the constructed path exists.
     * @return True if the path was successfully constructed, false otherwise.
     */
    bool constructPath(const QString &basePath, const QString &addition, QString &result, const QString &errorMsg, bool checkExists = true);

    QList<ScriptWorker*> scriptWorkers; ///< List of active ScriptWorker objects.
};

#endif // TASK_EXECUTOR_HPP