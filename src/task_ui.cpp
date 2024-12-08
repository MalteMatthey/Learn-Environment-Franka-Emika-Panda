#include "learn_environment/task_ui.h"
#include "learn_environment/subtask_item.h"

#include <QRegExp>
#include <QDebug>

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

    if (difficulty == "beginner") {
        newColor = "#8fb935";
    } else if (difficulty == "intermediate") {
        newColor = "#e09c3b";
    } else if (difficulty == "advanced") {
        newColor = "#e64747";
    } else {
        newColor = "#1d0e53"; // Default color
    }

    QRegExp colorRegex("background-color:\\s*[^;]+;");
    styleSheet.replace(colorRegex, "background-color: " + newColor + ";");
    difficultyLabel->setStyleSheet(styleSheet);
}

void TaskUI::setFolderLabelHtml(const QString &folder)
{
    QString folderHtml = R"(
        <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        <body style=" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;">
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px; line-height:60%;">Folder for this task:</p>
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px;"><span style="font-family:'monospace'; 
            color:'#444444';">)"
        + folder +
        R"(</span></p></body>
    )";
    folderLabel->setText(folderHtml);
}

// TASK_UI.CPP
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
        Subtask* subtask = &currentTask->subtasks[i]; // Zeiger erhalten

        // Erstelle ein neues SubtaskItem-Widget mit Zeiger
        SubtaskItem *itemWidget = new SubtaskItem(subtaskListWidget, subtask);

        // Setze den TaskManager, falls erforderlich
        itemWidget->setTaskManager(taskManager);

        // Erstelle ein QListWidgetItem und setze die Größenvorgabe
        QListWidgetItem *listItem = new QListWidgetItem(subtaskListWidget);
        listItem->setSizeHint(itemWidget->sizeHint());

        // Füge das SubtaskItem-Widget zur QListWidget hinzu
        subtaskListWidget->addItem(listItem);
        subtaskListWidget->setItemWidget(listItem, itemWidget);

        // Füge eine Linie zwischen den Widgets hinzu, außer nach dem letzten Element
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