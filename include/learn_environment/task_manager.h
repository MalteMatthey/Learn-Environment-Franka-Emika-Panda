#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QListWidget>
#include <QLabel>
#include <QPushButton>
#include "task.h"
#include "sidebar.h"
#include <QDir>
#include <QRegExp>
#include <vector>

class TaskManager : public QObject
{
    Q_OBJECT

public:
    TaskManager(Sidebar &sidebar, QListWidget *subtaskListWidget, QLabel *mainTitleLabel, QLabel *difficultyLabel, QLabel *folderLabel, QPushButton *nextButton, QPushButton *previousButton);
    void selectTask(int index);

private Q_SLOTS:
    void onNextButtonClicked();
    void onPreviousButtonClicked();

private:
    Sidebar &sidebar;
    QListWidget *subtaskListWidget;
    QLabel *mainTitleLabel;
    QLabel *difficultyLabel;
    QLabel *folderLabel;
    QPushButton *nextButton;
    QPushButton *previousButton;
    std::vector<Task> tasks;
    int currentTaskIndex;

    std::vector<Task> loadTasksJSON();
    void setTaskUI();
    void updateSubtaskItems();
    void addLineBetweenWidgets();
    void updateButtonStates();
    void setDifficultyLabelColor();
    void setFolderLabelHtml();
};

#endif // TASKMANAGER_H