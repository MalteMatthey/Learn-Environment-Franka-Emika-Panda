#include "learn_environment/script_worker.hpp"
#include "learn_environment/process_runner.hpp"

#include <QDebug>
#include <QFile>

namespace {
    const int CONVERT_JUPYTER_TIMEOUT_SECONDS = 30;
}

ScriptWorker::ScriptWorker(const QString &notebookPath,
                           const QString &convertedScriptPath,
                           const QString &evalScriptPath,
                           bool parallelizedEvaluation,
                           int timeout)
    : notebookPath(notebookPath),
      convertedScriptPath(convertedScriptPath),
      evalScriptPath(evalScriptPath),
      parallelizedEvaluationRequired(parallelizedEvaluation),
      timeoutSeconds(timeout),
      mainScriptFinished(false),
      evalScriptFinished(false),
      converter(this) {}

void ScriptWorker::startExecution() {
    if (!QFile::exists(notebookPath)) {
        Q_EMIT failed("Jupyter Notebook file not found");
        Q_EMIT finished();
        return;
    }

    convertAndExecuteNotebook();
   
    if (parallelizedEvaluationRequired) {
        evaluateScriptInParallel();
    }
}

void ScriptWorker::convertAndExecuteNotebook() {
    qDebug() << "Converting notebook:" << notebookPath;
    bool success = converter.convertNotebook(notebookPath);
    if (!success) {
        Q_EMIT failed("Notebook conversion failed.");
        Q_EMIT finished();
        return;
    }
    executeConvertedScript();
}

void ScriptWorker::executeConvertedScript() {
    qDebug() << "Executing converted Notebook...";
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {convertedScriptPath},
                                              timeoutSeconds,
                                              this,
                                              "Robot Script");
    processRunners.append(runner);

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            Q_EMIT failed("Converted script execution failed.");
            Q_EMIT finished();
        } else {
            mainScriptFinished = true;
            if (!parallelizedEvaluationRequired) {
                checkResult();
            }
            checkAndEmitFinished();
        }
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed("Converted script execution timed out.");
        Q_EMIT finished();
    });

    runner->start();
}

void ScriptWorker::evaluateScriptInParallel() {
    qDebug() << "Evaluating in parallel to the script...";
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {"-u", evalScriptPath},
                                              timeoutSeconds,
                                              this,
                                              "Evaluation");
    processRunners.append(runner);

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            Q_EMIT failed("Evaluation script failed.");
        }
        evalScriptFinished = true;
        checkAndEmitFinished();
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed("Evaluation script timed out.");
        evalScriptFinished = true;
        checkAndEmitFinished();
    });

    runner->start();
}

void ScriptWorker::checkResult() {
    qDebug() << "Checking result...";
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {evalScriptPath},
                                              timeoutSeconds,
                                              this,
                                              "Evaluation");
    processRunners.append(runner);

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            Q_EMIT failed("Result check failed.");
        }
        Q_EMIT finished();
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed("Result checking timed out.");
        Q_EMIT finished();
    });

    runner->start();
}

void ScriptWorker::checkAndEmitFinished() {
    if (mainScriptFinished && evalScriptFinished) {
        Q_EMIT finished();
    }
}

void ScriptWorker::forceStop() {
    for (ProcessRunner *runner : qAsConst(processRunners)) {
        runner->forceStop();
    }
}