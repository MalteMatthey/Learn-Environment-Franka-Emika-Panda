#include "learn_environment/task_ui.hpp"
#include "learn_environment/subtask_item.hpp"

#include <QRegExp>
#include <QDebug>

namespace {
    const char* BEGINNER = "beginner";
    const char* INTERMEDIATE = "intermediate";
    const char* ADVANCED = "advanced";
    const char* DEFAULT_COLOR = "#1d0e53";
    const char* BEGINNER_COLOR = "#8fb935";
    const char* INTERMEDIATE_COLOR = "#e09c3b";
    const char* ADVANCED_COLOR = "#e64747";
    const char* FOLDER_HTML_TEMPLATE = R"(
        <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        <body style=" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;">
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px; line-height:60%;">Folder for this task:</p>
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px;"><span style="font-family:'monospace'; 
            color:'#444444';">)";
}

TaskUI::TaskUI(QListWidget *subtaskListWidget, QLabel *mainTitleLabel,
               QLabel *difficultyLabel, QLabel *folderLabel,
               QPushButton *nextButton, QPushButton *previousButton,
               Sidebar &sidebar, QObject *parent)
    : QObject(parent),
      sidebar(sidebar),
      subtaskListWidget(subtaskListWidget),
      mainTitleLabel(mainTitleLabel),
      difficultyLabel(difficultyLabel),
      folderLabel(folderLabel),
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

    setDifficultyLabelColor(currentTask->difficulty);
    setFolderLabelHtml(currentTask->folder);

    nextButton->setEnabled(currentTaskIndex < tasks.size() - 1);
    previousButton->setEnabled(currentTaskIndex > 0);

    setSubtaskItems(currentTaskIndex);

    sidebar.selectTask(currentTaskIndex);
}

void TaskUI::setDifficultyLabelColor(const QString &difficulty)
{
    QString styleSheet = difficultyLabel->styleSheet();
    QString newColor;

    if (difficulty == BEGINNER) {
        newColor = BEGINNER_COLOR;
    } else if (difficulty == INTERMEDIATE) {
        newColor = INTERMEDIATE_COLOR;
    } else if (difficulty == ADVANCED) {
        newColor = ADVANCED_COLOR;
    } else {
        newColor = DEFAULT_COLOR;
    }

    QRegExp colorRegex("background-color:\\s*[^;]+;");
    styleSheet.replace(colorRegex, "background-color: " + newColor + ";");
    difficultyLabel->setStyleSheet(styleSheet);
}

void TaskUI::setFolderLabelHtml(const QString &folder)
{
    QString folderHtml = QString(FOLDER_HTML_TEMPLATE) + folder + R"(</span></p></body>)";
    folderLabel->setText(folderHtml);
}

void TaskUI::setSubtaskItems(int currentTaskIndex)
{
    if (tasks.isEmpty() || currentTaskIndex < 0 || currentTaskIndex >= tasks.size()) {
        qCritical() << "Invalid currentTaskIndex in setSubtaskItems:" << currentTaskIndex;
        return;
    }

    QSharedPointer<Task> currentTask = tasks[currentTaskIndex];
    subtaskListWidget->clear();
    int subtaskCount = currentTask->subtasks.size();
    for (int i = 0; i < subtaskCount; ++i) {
        Subtask* subtask = &currentTask->subtasks[i]; // Get pointer

        // Create a new SubtaskItem widget with pointer
        SubtaskItem *itemWidget = new SubtaskItem(subtaskListWidget, subtask);

        // Set the TaskManager if required
        itemWidget->setTaskManager(taskManager);

        // Create a QListWidgetItem and set the size hint
        QListWidgetItem *listItem = new QListWidgetItem(subtaskListWidget);
        listItem->setSizeHint(itemWidget->sizeHint());

        // Add the SubtaskItem widget to the QListWidget
        subtaskListWidget->addItem(listItem);
        subtaskListWidget->setItemWidget(listItem, itemWidget);

        // Add a line between widgets, except after the last element
        if (i < subtaskCount - 1) {
            addLineBetweenWidgets();
        }
    }
}

void TaskUI::updateSubtaskItemsUI()
{
    for (int i = 0; i < subtaskListWidget->count(); ++i) {
        QListWidgetItem *item = subtaskListWidget->item(i);
        SubtaskItem *subtaskItem = qobject_cast<SubtaskItem*>(subtaskListWidget->itemWidget(item));
        if (subtaskItem) {
            subtaskItem->updateUI();
        }
    }
}

void TaskUI::addLineBetweenWidgets()
{
    QWidget *container = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(10, 2, 10, 0);

    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("border: none; border-top: 1px solid #DDDDDD;");

    layout->addWidget(line);
    container->setLayout(layout);

    QListWidgetItem *item = new QListWidgetItem();
    item->setSizeHint(container->sizeHint());
    subtaskListWidget->addItem(item);
    subtaskListWidget->setItemWidget(item, container);
}