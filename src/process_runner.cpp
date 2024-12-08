#include "learn_environment/process_runner.h"

#include <QDebug>
#include <QRegularExpression>

ProcessRunner::ProcessRunner(const QString &program,
                             const QStringList &arguments,
                             int timeoutSeconds,
                             QObject *parent,
                             QString processName)
    : QObject(parent),
      program(program),
      arguments(arguments),
      timeoutSeconds(timeoutSeconds),
      processName(processName) {
    process = new QProcess(this);
    timer = new QTimer(this);
    timer->setSingleShot(true);
}

void ProcessRunner::start() {
    connect(process, &QProcess::readyReadStandardOutput, this, &ProcessRunner::onReadyReadStandardOutput);
    connect(process, &QProcess::readyReadStandardError, this, &ProcessRunner::onReadyReadStandardError);
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), this, &ProcessRunner::onProcessFinished);
    connect(timer, &QTimer::timeout, this, &ProcessRunner::onTimeout);

    process->start(program, arguments);
    timer->start(timeoutSeconds * 1000);
}

void ProcessRunner::forceStop() {
    if (process->state() != QProcess::NotRunning) {
        process->kill();
    }
    timer->stop();
}

void ProcessRunner::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus) {
    timer->stop();
    Q_EMIT finished(exitCode, exitStatus);
}

void ProcessRunner::onReadyReadStandardOutput() {
    QString output = process->readAllStandardOutput();

    // Strip ANSI escape codes
    QRegularExpression ansiRegex("\\x1B\\[[0-9;]*m");
    QString cleanOutput = output.remove(ansiRegex);

    Q_EMIT outputReady(cleanOutput);

    qDebug().noquote() << "Output of" << processName + ":" << cleanOutput.trimmed();
}

void ProcessRunner::onReadyReadStandardError() {
    QString error = process->readAllStandardError();

    // Strip ANSI escape codes
    QRegularExpression ansiRegex("\\x1B\\[[0-9;]*m");
    QString cleanError = error.remove(ansiRegex);

    Q_EMIT errorReady(cleanError);

    qWarning().noquote() << "Error in" << processName + ":" << cleanError.trimmed();
}

void ProcessRunner::onTimeout() {
    if (process->state() != QProcess::NotRunning) {
        process->kill();
        Q_EMIT timeout();
        qWarning().noquote() << "Process \"" + processName + "\" timed out.";
    }
}