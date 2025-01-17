#include "script_execution/script_worker.hpp"
#include "script_execution/process_runner.hpp"

#include <QDebug>
#include <QFile>

namespace {
    const int CONVERT_JUPYTER_TIMEOUT_SECONDS = 30;
    const QString JUPYTER_NOTEBOOK_NOT_FOUND = "<b>Jupyter Notebook file not found</b>";
    const QString NOTEBOOK_CONVERSION_FAILED = "<b>Notebook conversion failed.</b>";
    const QString ROBOT_SCRIPT_EXECUTION_FAILED = "<b>Robot script execution failed.</b>";
    const QString ROBOT_SCRIPT_FAILED_WITH_ERRORS = "<b>Your robot script failed with errors:</b><br>";
    const QString ROBOT_SCRIPT_EXECUTION_TIMED_OUT = "<b>Robot script execution timed out.</b>";
    const QString EVALUATION_SCRIPT_FAILED = "<b>Evaluation script failed.</b>";
    const QString EVALUATION_ERRORS = "<b>Evaluation errors. Contact the authors of the task:</b><br>";
    const QString SCRIPT_ERROR_MESSAGE = "<b>It looks like something is wrong in your script:</b><br>";
    const QString EVALUATION_SCRIPT_TIMED_OUT = "<b>Evaluation script timed out.</b>";
    const QString RESULT_CHECK_FAILED = "<b>Result check failed.</b>";
    const QString RESULT_CHECK_TIMED_OUT = "<b>Result checking timed out.</b>";
    const QString PYTHON_SCRIPT_EXECUTION_KILLED = "Python script execution was killed by the user.";
    const QString PYTHON_SCRIPT_EXECUTION_FAILED = "Python script execution failed with exit code %1.";
    const QString EVALUATION_FAILED_KEYWORD = "false";
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
        Q_EMIT failed(JUPYTER_NOTEBOOK_NOT_FOUND);
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
        Q_EMIT failed(NOTEBOOK_CONVERSION_FAILED);
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
            Q_EMIT failed(formattedError.isEmpty() ? ROBOT_SCRIPT_EXECUTION_FAILED : ROBOT_SCRIPT_FAILED_WITH_ERRORS + formattedError);
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
        Q_EMIT failed(ROBOT_SCRIPT_EXECUTION_TIMED_OUT);
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
            Q_EMIT failed(formattedError.isEmpty() ? EVALUATION_SCRIPT_FAILED : EVALUATION_ERRORS + formattedError);
        }
        if (evaluationOutput.trimmed().toLower().split('\n').contains(EVALUATION_FAILED_KEYWORD)) {
            QString formattedOutput = SCRIPT_ERROR_MESSAGE + formatMessage(evaluationOutput.trimmed(), true);
            Q_EMIT failed(formattedOutput);
        }
        evalScriptFinished = true;
        checkAndEmitFinished();
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed(EVALUATION_SCRIPT_TIMED_OUT);
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
            Q_EMIT failed(formattedError.isEmpty() ? RESULT_CHECK_FAILED : EVALUATION_ERRORS + formattedError);
        }
        if (evaluationOutput.trimmed().toLower().split('\n').contains(EVALUATION_FAILED_KEYWORD)) {
            QString formattedOutput = SCRIPT_ERROR_MESSAGE + formatMessage(evaluationOutput.trimmed(), true);
            Q_EMIT failed(formattedOutput);
        }
        Q_EMIT finished();
    });

    connect(runner, &ProcessRunner::timeout, this, [this]() {
        Q_EMIT failed(RESULT_CHECK_TIMED_OUT);
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
                Q_EMIT failed(PYTHON_SCRIPT_EXECUTION_KILLED);
            } else {
                Q_EMIT failed(QString(PYTHON_SCRIPT_EXECUTION_FAILED).arg(exitCode));
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
        if (fromEval && (trimmedLine.toLower() == "true" 
                      || trimmedLine.toLower() == EVALUATION_FAILED_KEYWORD 
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