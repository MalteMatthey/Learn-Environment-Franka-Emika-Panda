#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVector>
#include <QSharedPointer>
#include <QWeakPointer>

// Forward declaration of Task struct
struct Task;

/**
 * @enum SubtaskStatus
 * @brief Represents the status of a subtask.
 */
enum class SubtaskStatus {
    Inactive, ///< The subtask is inactive (state where it can't be executed).
    Ready, ///< The subtask is ready to be executed.
    Queued, ///< The subtask is queued for execution.
    Running ///< The subtask is currently being executed.
};

/**
 * @struct Subtask
 * @brief Represents a subtask within a task.
 *
 * The Subtask struct contains information about a subtask, including its title,
 * description, file paths, difficulty, and status. It also includes optional fields
 * for timeout and parallelized evaluation.
 */
struct Subtask {
    QString title; ///< The title of the subtask.
    QString description; ///< The description of the subtask.
    QString file; ///< The file path of the subtask.
    QString solutionFilePath; ///< The file path to the solution of the subtask.
    QString evaluationFilePath; ///< The file path to the evaluation script of the subtask.

    int timeoutSeconds = 60; ///< The timeout for the subtask execution in seconds.
    bool parallelizedEvaluationRequired = false; ///< Whether parallelized evaluation is required.

    QWeakPointer<Task> parentTask; ///< Weak pointer to the parent task.
    SubtaskStatus status = SubtaskStatus::Inactive; ///< The status of the subtask.
};

/**
 * @struct Task
 * @brief Represents a task containing multiple subtasks.
 *
 * The Task struct contains information about a task, including its title,
 * description, folder, difficulty, and a list of subtasks. It also includes
 * an optional field indicating whether previous subtasks are required.
 */
struct Task {
    QString title; ///< The title of the task.
    QString description; ///< The description of the task.
    QString folder; ///< The folder path of the task.
    QString difficulty; ///< The difficulty level of the task.
    QVector<Subtask> subtasks; ///< The list of subtasks within the task.

    bool previousSubtasksRequired = false; ///< Whether previous subtasks are required.
};

#endif // TASK_H