#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QObject>
#include <QPushButton>
#include <QVector>
#include <QSharedPointer>
#include "task.h"
#include "task_executor.h"

class TaskUI;

class TaskManager : public QObject
{
    Q_OBJECT

public:
    TaskManager(TaskUI *taskUI, QPushButton *nextButton, QPushButton *previousButton, QObject *parent = nullptr);

    void startStopSubtask(Subtask &subtask);

public Q_SLOTS:
    void startStopSubtask(const Subtask &subtask);

private Q_SLOTS:
    void onNextButtonClicked();
    void onPreviousButtonClicked();
    void selectTask(int index);

    void onTaskExecutionStarted();
    void onTaskExecutionFinished();
    void onTaskExecutionFailed(const QString &error);

private:
    TaskUI *taskUI;
    TaskExecutor *taskExecutor;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QVector<QSharedPointer<Task>> tasks;
    int currentTaskIndex;

    void setTaskUI();
};

#endif // TASKMANAGER_H