#ifndef SCRIPT_WORKER_HPP
#define SCRIPT_WORKER_HPP

#include <QObject>
#include <QString>
#include <QProcess>
#include <QList>

#include "notebook_converter.hpp"

class ProcessRunner;

/**
 * @class ScriptWorker
 * @brief Manages the execution of scripts for tasks.
 *
 * The ScriptWorker class is responsible for converting and executing Jupyter notebooks,
 * as well as evaluating the results. It handles the execution state and emits signals
 * based on the success or failure of the scripts.
 */
class ScriptWorker : public QObject {
    Q_OBJECT

public:
    /**
     * @brief Constructs a ScriptWorker object.
     * @param notebookPath Path to the Jupyter notebook file.
     * @param convertedScriptPath Path to the converted script.
     * @param evalScriptPath Path to the evaluation script.
     * @param parallelizedEvaluation Whether the evaluation should run in parallel to the main script.
     * @param timeoutSeconds Timeout for script execution in seconds.
     */
    ScriptWorker(const QString &notebookPath,
                 const QString &convertedScriptPath,
                 const QString &evalScriptPath,
                 bool parallelizedEvaluation,
                 int timeoutSeconds);

public Q_SLOTS:
    /**
     * @brief Starts the execution of the scripts.
     */
    void startExecution();

    /**
     * @brief Forces the stop of all running scripts.
     */
    void forceStop();

    /**
     * @brief Executes a given Python script.
     */
    void executePythonScript(const QString &scriptPath, const QString &name);

Q_SIGNALS:
    /**
     * @brief Signal emitted when the script execution finishes.
     */
    void finished();

    /**
     * @brief Signal emitted when the script execution fails.
     * @param error The error message.
     */
    void failed(const QString &error);

private:
    /**
     * @brief Converts and executes the Jupyter notebook.
     */
    void convertAndExecuteNotebook();

    /**
     * @brief Executes the converted script.
     */
    void executeConvertedScript();

    /**
     * @brief Evaluates the script in parallel to the execution of the main script.
     */
    void evaluateScriptInParallel();

    /**
     * @brief Checks the result of the script execution.
     */
    void checkResult();

    /**
     * @brief Checks if both main and evaluation scripts are finished and emits the finished signal.
     */
    void checkAndEmitFinished();

    QString notebookPath; ///< Path to the Jupyter notebook file.
    QString convertedScriptPath; ///< Path to the converted script.
    QString evalScriptPath; ///< Path to the evaluation script.
    bool parallelizedEvaluationRequired; ///< Whether the evaluation should run in parallel to the main script.
    int timeoutSeconds; ///< Timeout for script execution in seconds.

    bool mainScriptFinished; ///< Indicates if the main script has finished.
    bool evalScriptFinished; ///< Indicates if the evaluation script has finished.

    QList<ProcessRunner*> processRunners; ///< List of active ProcessRunner objects.
    NotebookConverter converter; ///< Instance of NotebookConverter.
};

#endif // SCRIPT_WORKER_HPP