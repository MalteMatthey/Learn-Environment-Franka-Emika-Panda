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
    // qDebug() << "Converting notebook:" << notebookPath;
    bool success = converter.convertNotebook(notebookPath);
    if (!success) {
        Q_EMIT failed("Notebook conversion failed.");
        Q_EMIT finished();
        return;
    }
    executeConvertedScript();
}

void ScriptWorker::executeConvertedScript() {
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {"-u", convertedScriptPath},
                                              timeoutSeconds,
                                              this,
                                              "Robot Script");
    processRunners.append(runner);

    connect(runner, &ProcessRunner::errorReady, this, [&](const QString &error) {
        errorOutput += error + "\n";
    });

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            QString formattedError = formatMessage(errorOutput.trimmed());
            Q_EMIT failed(formattedError.isEmpty() ? "<b>Robot script execution failed.</b>" : "<b>Your robot script failed with errors:</b><br>" + formattedError);
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
        Q_EMIT failed("Robot script execution timed out.");
        Q_EMIT finished();
    });

    runner->start();
}

void ScriptWorker::evaluateScriptInParallel() {
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {evalScriptPath},
                                              timeoutSeconds,
                                              this,
                                              "Evaluation");
    processRunners.append(runner);

    connect(runner, &ProcessRunner::errorReady, this, [&](const QString &error) {
        errorOutput += error + "\n";
    });

    connect(runner, &ProcessRunner::outputReady, this, [&](const QString &output) {
        evaluationOutput += output + "\n";
    });

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            QString formattedError = formatMessage(errorOutput.trimmed());
            Q_EMIT failed(formattedError.isEmpty() ? "<b>Evaluation script failed.</b>" : "<b>Evaluation errors. Contact the authors of the task:</b><br>" + formattedError);
        }
        if (evaluationOutput.trimmed().split('\n').contains("false")) {
            QString formattedOutput = "<b>It looks like something is wrong in your script:</b><br>" + formatMessage(evaluationOutput.trimmed(), true);
            Q_EMIT failed(formattedOutput);
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
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {"-u", evalScriptPath},
                                              timeoutSeconds,
                                              this,
                                              "Result Check");
    processRunners.append(runner);

    connect(runner, &ProcessRunner::errorReady, this, [&](const QString &error) {
        errorOutput += error + "\n";
    });

    connect(runner, &ProcessRunner::outputReady, this, [&](const QString &output) {
        evaluationOutput += output + "\n";
    });

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            QString formattedError = formatMessage(errorOutput.trimmed());
            Q_EMIT failed(formattedError.isEmpty() ? "<b>Result check failed.</b>" : "</b>Evaluation errors. Contact the authors of the task:</b><br>" + formattedError);
        }
        if (evaluationOutput.trimmed().split('\n').contains("false")) {
            QString formattedOutput = "<b>It looks like something is wrong in your script:</b><br>" + formatMessage(evaluationOutput.trimmed(), true);
            Q_EMIT failed(formattedOutput);
        }
        Q_EMIT finished();
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed("Result checking timed out.");
        Q_EMIT finished();
    });

    runner->start();
}

void ScriptWorker::executePythonScript(const QString &scriptPath, const QString &name) {
    qDebug() << "Executing Python script:" << scriptPath;
    ProcessRunner *runner = new ProcessRunner("python3",
                                              {"-u", scriptPath},
                                              timeoutSeconds,
                                              this,
                                              name);
    processRunners.append(runner);

    connect(runner, &ProcessRunner::finished, this, [this](int exitCode, QProcess::ExitStatus) {
        if (exitCode != 0) {
            if (exitCode == 9) {
                Q_EMIT failed("Python script execution was killed by the user.");
            } else {
                Q_EMIT failed(QString("Python script execution failed with exit code %1.").arg(exitCode));
            }
        }
        Q_EMIT finished();
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed("Python script execution timed out.");
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

QString ScriptWorker::formatMessage(const QString &msg, bool fromEval) {
    QStringList lines = msg.split('\n');
    QStringList selectedLines;
    QString formatted;
    int totalChars = 0;
    int maxLines = 4;

    // Iterate from the end to get the latest lines
    for (int i = lines.size() - 1; i >= 0; --i) {
        QString trimmedLine = lines[i].trimmed();
        if (trimmedLine.isEmpty()) {
            continue;
        }
        if (fromEval && (trimmedLine == "true" 
                      || trimmedLine == "false" 
                      || trimmedLine.startsWith("[ INFO]") 
                      || trimmedLine.startsWith("[ WARN]"))) {
            continue;
        }
        if (selectedLines.size() >= maxLines || totalChars + trimmedLine.length() + 1 > 256) {
            if (fromEval) {
                formatted = "Last evaluation errors, for more check the logs in the terminal:<br>" + formatted;
            } else {
                formatted = "Last error messages, for more check the logs in the terminal:<br>" + formatted;
            }
            break;
        }
        selectedLines.prepend(trimmedLine);
        totalChars += trimmedLine.length() + 1;
    }

    // Join the selected lines in chronological order
    formatted = selectedLines.join("<br>");

    if (!formatted.isEmpty()) {
        if (fromEval) {
            formatted = "Last evaluation errors, for more check the logs in the terminal:<br>" + formatted;
        } else {
            formatted = "Last error messages, for more check the logs in the terminal:<br>" + formatted;
        }
    }

    return formatted;
}