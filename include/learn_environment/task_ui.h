#ifndef TASKUI_H
#define TASKUI_H

#include <QObject>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include "sidebar.h"
#include "task.h"
#include "task_manager.h"

class TaskUI : public QObject
{
    Q_OBJECT

public:
    TaskUI(QListWidget *subtaskListWidget, QLabel *mainTitleLabel,
           QLabel *difficultyLabel, QLabel *folderLabel,
           QPushButton *nextButton, QPushButton *previousButton,
           Sidebar &sidebar, QObject *parent = nullptr);

    void initializeUI(const QVector<QSharedPointer<Task>> &tasks);
    void setTaskUI(int currentTaskIndex);
    void updateSubtaskItemsUI();
    void setTaskManager(TaskManager *manager);

Q_SIGNALS:
    void taskSelected(int index);

private:
    Sidebar &sidebar;
    QListWidget *subtaskListWidget;
    QLabel *mainTitleLabel;
    QLabel *difficultyLabel;
    QLabel *folderLabel;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QVector<QSharedPointer<Task>> tasks;
    TaskManager *taskManager;

    void setDifficultyLabelColor(const QString &difficulty);
    void setFolderLabelHtml(const QString &folder);
    void setSubtaskItems(int currentTaskIndex);
    void addLineBetweenWidgets();
};

#endif // TASKUI_H