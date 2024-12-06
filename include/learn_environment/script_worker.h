#ifndef SCRIPT_WORKER_H
#define SCRIPT_WORKER_H

#include <QObject>
#include <QString>
#include <QProcess>
#include <QList>

class ProcessRunner;

class ScriptWorker : public QObject {
    Q_OBJECT

public:
    ScriptWorker(const QString &notebookPath,
                 const QString &convertScriptPath,
                 const QString &convertedScriptPath,
                 const QString &evalScriptPath,
                 bool parallelizedEvaluation,
                 int timeoutSeconds);

public Q_SLOTS:
    void startExecution();
    void forceStop();

Q_SIGNALS:
    void finished();
    void failed(const QString &error);

private:
    void convertAndExecuteNotebook();
    void executeConvertedScript();
    void evaluateScriptInParallel();
    void checkResult();

    QString notebookPath;
    QString convertScriptPath;
    QString convertedScriptPath;
    QString evalScriptPath;
    bool parallelizedEvaluationRequired;
    int timeoutSeconds;

    QList<ProcessRunner*> processRunners;
};

#endif // SCRIPT_WORKER_H