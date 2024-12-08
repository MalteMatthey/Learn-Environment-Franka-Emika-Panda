#ifndef PROCESS_RUNNER_H
#define PROCESS_RUNNER_H

#include <QObject>
#include <QProcess>
#include <QTimer>

/**
 * @class ProcessRunner
 * @brief Manages the execution of an external process with a timeout.
 *
 * The ProcessRunner class is responsible for starting an external process,
 * capturing its output and errors, and enforcing a timeout for its execution.
 */
class ProcessRunner : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a ProcessRunner object.
     * @param program The program to be executed.
     * @param arguments The arguments to be passed to the program.
     * @param timeoutSeconds The timeout for the process execution in seconds.
     * @param parent Pointer to the parent QObject.
     * @param processName The name of the process (for logging purposes).
     */
    ProcessRunner(const QString &program,
                  const QStringList &arguments,
                  int timeoutSeconds,
                  QObject *parent = nullptr,
                  QString processName = "process");

    /**
     * @brief Starts the execution of the process.
     */
    void start();

Q_SIGNALS:
    /**
     * @brief Signal emitted when the process finishes.
     * @param exitCode The exit code of the process.
     * @param exitStatus The exit status of the process.
     */
    void finished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * @brief Signal emitted when the process produces output.
     * @param output The output produced by the process.
     */
    void outputReady(const QString &output);

    /**
     * @brief Signal emitted when the process produces an error.
     * @param error The error produced by the process.
     */
    void errorReady(const QString &error);

    /**
     * @brief Signal emitted when the process times out.
     */
    void timeout();

public Q_SLOTS:
    /**
     * @brief Forces the stop of the running process.
     */
    void forceStop();

private Q_SLOTS:
    /**
     * @brief Handles the process finished event.
     * @param exitCode The exit code of the process.
     * @param exitStatus The exit status of the process.
     */
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

    /**
     * @brief Handles the ready read standard output event.
     */
    void onReadyReadStandardOutput();

    /**
     * @brief Handles the ready read standard error event.
     */
    void onReadyReadStandardError();

    /**
     * @brief Handles the timeout event.
     */
    void onTimeout();

private:
    QProcess *process; ///< Pointer to the QProcess object.
    QTimer *timer; ///< Pointer to the QTimer object.
    QString program; ///< The program to be executed.
    QStringList arguments; ///< The arguments to be passed to the program.
    int timeoutSeconds; ///< The timeout for the process execution in seconds.
    QString processName; ///< The name of the process (for logging purposes).
};

#endif // PROCESS_RUNNER_H