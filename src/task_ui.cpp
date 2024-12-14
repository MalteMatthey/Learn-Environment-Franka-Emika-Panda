#include "learn_environment/task_ui.hpp"
#include "learn_environment/subtask_item.hpp"

#include <QRegExp>
#include <QDebug>

namespace {
    const char* FOLDER_HTML_TEMPLATE = R"(
        <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        <body style=" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;">
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px; line-height:60%;">Folder for this task:</p>
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px;"><span style="font-family:'monospace'; 
            color:'#444444';">)";
}

TaskUI::TaskUI(QVBoxLayout *subtaskListLayout, QLabel *mainTitleLabel,
               QLabel *difficultyLabel, QLabel *folderLabel, QLabel *topicLabel,
               QPushButton *nextButton, QPushButton *previousButton,
               Sidebar &sidebar, QObject *parent)
    : QObject(parent),
      sidebar(sidebar),
      subtaskListLayout(subtaskListLayout),
      mainTitleLabel(mainTitleLabel),
      difficultyLabel(difficultyLabel),
      folderLabel(folderLabel),
      topicLabel(topicLabel),
      nextButton(nextButton),
      previousButton(previousButton)
{ }

void TaskUI::setTaskManager(TaskManager *manager)
{
    taskManager = manager;
}

void TaskUI::initializeUI(const QVector<QSharedPointer<Task>> &loadedTasks)
{
    tasks = loadedTasks;
    if (tasks.isEmpty()) {
        qCritical() << "No tasks loaded. Exiting TaskUI initialization.";
        return;
    }

    sidebar.fillSidebarWithTasks(tasks);

    connect(&sidebar, &Sidebar::taskSelected, this, &TaskUI::taskSelected);
}

void TaskUI::setTaskUI(int currentTaskIndex)
{
    if (tasks.isEmpty() || currentTaskIndex < 0 || currentTaskIndex >= tasks.size()) {
        qCritical() << "Invalid currentTaskIndex:" << currentTaskIndex;
        return;
    }

    QSharedPointer<Task> currentTask = tasks[currentTaskIndex];
    mainTitleLabel->setText(currentTask->title);
    difficultyLabel->setText(currentTask->difficulty);
    QString styleSheet = difficultyLabel->styleSheet();
    QRegExp colorRegex("background-color:\\s*[^;]+;");
    styleSheet.replace(colorRegex, "background-color: " + currentTask->difficultyHexColor + ";");
    difficultyLabel->setStyleSheet(styleSheet);

    folderLabel->setText(QString(FOLDER_HTML_TEMPLATE) + currentTask->folder + R"(</span></p></body>)");

    topicLabel->setText(currentTask->topic);

    nextButton->setEnabled(currentTaskIndex < tasks.size() - 1);
    previousButton->setEnabled(currentTaskIndex > 0);

    setSubtaskItems(currentTaskIndex);

    sidebar.selectTask(currentTaskIndex);
}

void TaskUI::setSubtaskItems(int currentTaskIndex)
{
    if (tasks.isEmpty() || currentTaskIndex < 0 || currentTaskIndex >= tasks.size()) {
        qCritical() << "Invalid currentTaskIndex in setSubtaskItems:" << currentTaskIndex;
        return;
    }

    QSharedPointer<Task> currentTask = tasks[currentTaskIndex];
    QLayoutItem *child;
    while ((child = subtaskListLayout->takeAt(0)) != nullptr) {
        if (child->widget()) {
            child->widget()->deleteLater();
        }
        delete child;
    }
    int subtaskCount = currentTask->subtasks.size();
    for (int i = 0; i < subtaskCount; ++i) {
        Subtask* subtask = &currentTask->subtasks[i];

        // Create a new SubtaskItem widget with pointer
        SubtaskItem *itemWidget = new SubtaskItem(subtaskListLayout->parentWidget(), subtask);
        itemWidget->setTaskManager(taskManager);
        itemWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum); // Set minimal vertical size

        // Add the SubtaskItem widget to the QVBoxLayout
        subtaskListLayout->addWidget(itemWidget);

        // Add a line between widgets, except after the last element
        if (i < subtaskCount - 1) {
            addLineBetweenWidgets();
        }
    }

    // Add spacer at the bottom to push items to the top
    subtaskListLayout->addStretch();
}

void TaskUI::updateSubtaskItemsUI()
{
    for (int i = 0; i < subtaskListLayout->count(); ++i) {
        QWidget *widget = subtaskListLayout->itemAt(i)->widget();
        SubtaskItem *subtaskItem = qobject_cast<SubtaskItem*>(widget);
        if (subtaskItem) {
            subtaskItem->updateUI();
        }
    }
}

void TaskUI::addLineBetweenWidgets()
{
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("border: none; border-top: 1px solid #DDDDDD;");

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(10, 2, 10, 0);
    QWidget *container = new QWidget();
    container->setLayout(layout);
    layout->addWidget(line);

    subtaskListLayout->addWidget(container);
}