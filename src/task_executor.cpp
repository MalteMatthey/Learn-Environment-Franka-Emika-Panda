// TASK_EXECUTOR.CPP

#include "learn_environment/task_executor.h"
#include "learn_environment/script_worker.h"

#include <QThread>
#include <QFile>
#include <ros/package.h>
#include <ros/ros.h>

TaskExecutor::TaskExecutor(QObject *parent) : QObject(parent) {}

void TaskExecutor::executeTask(const Subtask &subtask) {
    Q_EMIT taskExecutionStarted();

    // Attempt to lock the QWeakPointer to obtain a QSharedPointer
    QSharedPointer<Task> parentTaskPtr = subtask.parentTask.lock();
    if (!parentTaskPtr) {
        Q_EMIT taskExecutionFailed("Parent task is no longer available.");
        return;
    }

    QString packagePath;
    try {
        packagePath = QString::fromStdString(ros::package::getPath("learn_environment"));
    } catch (...) {
        Q_EMIT taskExecutionFailed("An error occurred while getting the package path");
        return;
    }

    QString notebookPath, convertScriptPath, convertedScriptPath, evalScriptPath;

    if (!constructPath(packagePath, parentTaskPtr->folder + subtask.file, notebookPath, "constructing the notebook path") ||
        !constructPath(packagePath, "/converter/convert.py", convertScriptPath, "constructing the convert script path") ||
        !constructPath(packagePath, "/converter/converted.py", convertedScriptPath, "constructing the converted script path", false) ||
        !constructPath(packagePath, subtask.evaluationFilePath, evalScriptPath, "constructing the evaluation script path")) {
        return;
    }

    QThread *thread = new QThread;
    ScriptWorker *worker = new ScriptWorker(notebookPath, convertScriptPath, convertedScriptPath, evalScriptPath, subtask.parallelizedEvaluationRequired, subtask.timeoutSeconds);
    worker->moveToThread(thread);

    // Track the active worker
    scriptWorkers.append(worker);

    connect(thread, &QThread::started, worker, &ScriptWorker::startExecution);
    connect(worker, &ScriptWorker::finished, thread, &QThread::quit);
    connect(worker, &ScriptWorker::finished, worker, &ScriptWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // Forward signals from the worker to handle execution status
    connect(worker, &ScriptWorker::finished, this, [this, worker]() {
        scriptWorkers.removeOne(worker);
        Q_EMIT taskExecutionFinished();
    });
    connect(worker, &ScriptWorker::failed, this, [this, worker](const QString &error) {
        scriptWorkers.removeOne(worker);
        Q_EMIT taskExecutionFailed(error);
    });

    thread->start();
}

void TaskExecutor::forceStop() {
    for (ScriptWorker *worker : scriptWorkers) {
        worker->forceStop();
    }
}

bool TaskExecutor::constructPath(const QString& basePath, const QString& addition, QString& result, const QString& errorMsg, bool checkExists) {
    try {
        result = basePath + addition;
        if (!QFile::exists(result) && checkExists) {
            Q_EMIT taskExecutionFailed("The file " + result + " does not exist");
            return false;
        }
        return true;
    } catch (...) {
        Q_EMIT taskExecutionFailed("An error occurred while " + errorMsg.toLower());
        return false;
    }
}