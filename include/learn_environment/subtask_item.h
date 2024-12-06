#ifndef SUBTASK_ITEM_H
#define SUBTASK_ITEM_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "task.h"
#include "task_manager.h"

class SubtaskItem : public QWidget {
    Q_OBJECT

public:
    explicit SubtaskItem(QWidget *parent = nullptr, const Subtask &subtask = Subtask());
    void updateUI();

    void setTaskManager(TaskManager *manager);

    Subtask getSubtask() const { return subtask; } 

Q_SIGNALS:
    void subtaskStartStopRequested(const Subtask &subtask);

private Q_SLOTS:
    void handleStartButtonClick();

private:
    void setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText);

    TaskManager *taskManager;
    QString headerText;
    QString linkText;
    QString bodyText;
    Subtask subtask;
    QPushButton *playButton;
    QPushButton *resetButton;
    QPushButton *solutionButton;
};

#endif // SUBTASK_ITEM_H