#ifndef TASK_EXECUTOR_H
#define TASK_EXECUTOR_H

#include <QObject>
#include <QString>
#include <QSharedPointer>
#include "task.h"

class ScriptWorker;

class TaskExecutor : public QObject {
    Q_OBJECT

public:
    explicit TaskExecutor(QObject *parent = nullptr);
    void executeTask(const Subtask &subtask);
    void forceStop();

Q_SIGNALS:
    void taskExecutionStarted();
    void taskExecutionFinished();
    void taskExecutionFailed(const QString &error);

private:
    bool constructPath(const QString &basePath, const QString &addition, QString &result, const QString &errorMsg, bool checkExists = true);

    QList<ScriptWorker*> scriptWorkers;
};

#endif // TASK_EXECUTOR_H