#ifndef PROCESS_RUNNER_H
#define PROCESS_RUNNER_H

#include <QObject>
#include <QProcess>
#include <QTimer>

class ProcessRunner : public QObject {
    Q_OBJECT

public:
    ProcessRunner(const QString &program,
                  const QStringList &arguments,
                  int timeoutSeconds,
                  QObject *parent = nullptr,
                  QString processName = "process");

    void start();

Q_SIGNALS:
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void outputReady(const QString &output);
    void errorReady(const QString &error);
    void timeout();

public Q_SLOTS:
    void forceStop();

private Q_SLOTS:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();
    void onTimeout();

private:
    QProcess *process;
    QTimer *timer;
    QString program;
    QStringList arguments;
    int timeoutSeconds;
    QString processName;
};

#endif // PROCESS_RUNNER_H