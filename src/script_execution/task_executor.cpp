#include "script_execution/task_executor.hpp"
#include "script_execution/script_worker.hpp"
#include "common/folder_structure_constants.hpp"

#include <QThread>
#include <QFile>

TaskExecutor::TaskExecutor(QObject *parent) : QObject(parent) {}

void TaskExecutor::executeTask(const Subtask &subtask, bool startSolution) {
    Q_EMIT taskExecutionStarted();

    QSharedPointer<Task> parentTaskPtr = subtask.parentTask.lock();
    if (!parentTaskPtr) {
        Q_EMIT taskExecutionFailed("Parent task is no longer available.");
        return;
    }

    QString fullNotebookPath, fullConvertScriptPath, fullConvertedScriptPath, fullEvalScriptPath;

    if (!constructPath("", // subtask paths are already absolute
                       startSolution ? subtask.solutionFilePath : subtask.filePath,
                       fullNotebookPath,
                       "constructing the notebook path") ||
        !constructPath(FolderStructureConstants::getPackagePath(),
                       FolderStructureConstants::CONVERTED_SCRIPT_PATH,
                       fullConvertedScriptPath,
                       "constructing the converted script path", 
                       false) ||
        !constructPath("", // subtask paths are already absolute
                       subtask.evaluationFilePath,
                       fullEvalScriptPath, 
                       "constructing the evaluation script path")) {
        return;
    }

    QThread *thread = new QThread;
    ScriptWorker *worker = new ScriptWorker(fullNotebookPath, fullConvertedScriptPath, fullEvalScriptPath, subtask.parallelizedEvaluationRequired, subtask.timeoutSeconds);
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

void TaskExecutor::resetRobot() {
    Q_EMIT resetRobotStarted();

    QString resetRobotScriptPath;
    if (!constructPath(FolderStructureConstants::getPackagePath(),
                       FolderStructureConstants::RESET_ROBOT_SCRIPT_PATH,
                       resetRobotScriptPath,
                       "constructing the reset robot script path")) {
        return;
    }

    QThread *thread = new QThread;
    ScriptWorker *worker = new ScriptWorker("", "", "", false, 30);
    worker->moveToThread(thread);

    scriptWorkers.append(worker);

    connect(thread, &QThread::started, worker, [worker, resetRobotScriptPath]() {
        worker->executePythonScript(resetRobotScriptPath, "Reset Robot Script");
    });
    connect(worker, &ScriptWorker::finished, thread, &QThread::quit);
    connect(worker, &ScriptWorker::finished, worker, &ScriptWorker::deleteLater);
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    // Forward signals from the worker to handle reset status
    
    connect(worker, &ScriptWorker::finished, this, [this, worker]() {
        scriptWorkers.removeOne(worker);
        Q_EMIT resetRobotFinished();
    });
    connect(worker, &ScriptWorker::failed, this, [this, worker](const QString &error) {
        scriptWorkers.removeOne(worker);
        Q_EMIT resetRobotFailed(error);
    });

    thread->start();
}

bool TaskExecutor::constructPath(const QString &basePath, const QString &addition, QString &result, const QString &errorMsg, bool checkExists)
{
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