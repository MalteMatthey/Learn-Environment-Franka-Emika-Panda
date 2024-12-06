#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QVector>
#include <QSharedPointer>

struct Task; // Forward declaration

enum class SubtaskStatus {
    Inactive,
    Ready,
    Queued,
    Running
};

struct Subtask {
    QString title;
    QString description;
    QString file;
    QString difficulty;
    QString solutionFilePath;
    QString evaluationFilePath;

    // Optional fields
    int timeoutSeconds = 60;
    bool parallelizedEvaluationRequired = false;

    // Internal fields
    QSharedPointer<Task> parentTask;
    SubtaskStatus status = SubtaskStatus::Inactive;
};

struct Task {
    QString title;
    QString description;
    QString folder;
    QString difficulty;
    QVector<Subtask> subtasks;

    // Optional fields
    bool previousSubtasksRequired = false;
};

#endif // TASK_H