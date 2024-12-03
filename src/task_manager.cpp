#include "learn_environment/task_manager.h"
#include "learn_environment/subtask_item.h"
#include "learn_environment/sidebar.h"
#include <nlohmann/json.hpp>
#include <QFile>
#include <QDebug>
#include <QIODevice>
#include <QTextStream>
#include <iostream>

using json = nlohmann::json;

TaskManager::TaskManager(Sidebar &sidebar, QListWidget *subtaskListWidget, QLabel *mainTitleLabel, QLabel *difficultyLabel, QLabel *folderLabel, QPushButton *nextButton, QPushButton *previousButton)
    : sidebar(sidebar), subtaskListWidget(subtaskListWidget), mainTitleLabel(mainTitleLabel), difficultyLabel(difficultyLabel), folderLabel(folderLabel), nextButton(nextButton), previousButton(previousButton)
{
    tasks = loadTasksJSON();

    currentTaskIndex = 0;

    sidebar.fillSidebarWithTasks(tasks);

    setTaskUI();

    connect(nextButton, &QPushButton::clicked, this, &TaskManager::onNextButtonClicked);
    connect(previousButton, &QPushButton::clicked, this, &TaskManager::onPreviousButtonClicked);

    // Connect the sidebar's taskSelected signal to the selectTask slot
    connect(&sidebar, &Sidebar::taskSelected, this, &TaskManager::selectTask);
}

void TaskManager::selectTask(int index)
{
    if (index >= 0 && index < static_cast<int>(tasks.size())) {
        currentTaskIndex = index;
        setTaskUI();
    } else {
        qWarning() << "Invalid task index selected:" << index;
    }
}

std::vector<Task> TaskManager::loadTasksJSON()
{
    QFile file(":/tasks/task_definitions.json");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cerr << "Could not open the file!" << std::endl;
        return {};
    }

    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();

    json jsonData;
    try {
        jsonData = json::parse(jsonString.toStdString());
    } catch (const json::parse_error& e) {
        std::cerr << "JSON parse error: " << e.what() << std::endl;
        return {};
    }

    std::vector<Task> tasks;

    for (const auto& taskJson : jsonData["tasks"]) {
        Task task;
        task.title = taskJson["title"];
        task.description = taskJson["description"];
        task.folder = taskJson["folder"];
        task.difficulty = taskJson["difficulty"];
        
        for (const auto& subtaskJson : taskJson["subtasks"]) {
            Subtask subtask;
            subtask.title = subtaskJson["title"];
            subtask.description = subtaskJson["description"];
            subtask.file = subtaskJson["file"];
            subtask.difficulty = subtaskJson["difficulty"];
            subtask.solution_file = subtaskJson["solution_file"];
            subtask.evaluation_file = subtaskJson["evaluation_file"];
            task.subtasks.push_back(subtask);
        }
        tasks.push_back(task);
    }
    return tasks;
}

void TaskManager::setTaskUI()
{
    mainTitleLabel->setText(QString::fromStdString(tasks[currentTaskIndex].title));
    difficultyLabel->setText(QString::fromStdString(tasks[currentTaskIndex].difficulty));

    setDifficultyLabelColor();
    setFolderLabelHtml();

    nextButton->setEnabled(currentTaskIndex < tasks.size() - 1);
    previousButton->setEnabled(currentTaskIndex > 0);

    updateSubtaskItems();
    updateButtonStates();
    sidebar.selectTask(currentTaskIndex);
}

void TaskManager::setDifficultyLabelColor()
{
    QString difficulty = QString::fromStdString(tasks[currentTaskIndex].difficulty);
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

void TaskManager::setFolderLabelHtml()
{
    QString folderHtml = R"(
        <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        <body style=" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;">
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px; line-height:60%;">Folder for this task:</p>
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;"><span style="font-family:'monospace'; color:'#444444';">)"
        + QString::fromStdString(tasks[currentTaskIndex].folder) + 
        R"(</span></p></body>
    )";
    folderLabel->setText(folderHtml);
}

void TaskManager::updateSubtaskItems()
{
    subtaskListWidget->clear();
    int subtaskCount = tasks[currentTaskIndex].subtasks.size();
    for (int i = 0; i < subtaskCount; ++i) {
        const auto& subtask = tasks[currentTaskIndex].subtasks[i];
        SubtaskItem *subtaskItem = new SubtaskItem(
            nullptr,
            QString::fromStdString(subtask.title),
            QString::fromStdString(subtask.file),
            QString::fromStdString(subtask.description)
        );

        QListWidgetItem *item = new QListWidgetItem();
        item->setSizeHint(subtaskItem->sizeHint());
        subtaskListWidget->addItem(item);
        subtaskListWidget->setItemWidget(item, subtaskItem);

        // Add line between items, but not after the last item
        if (i < subtaskCount - 1) {
            addLineBetweenWidgets();
        }
    }
}

void TaskManager::addLineBetweenWidgets() {
    QWidget *container = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(container);
    // Somehow the line is vertically misaligned, so we need to set the top margin to 0
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

void TaskManager::onNextButtonClicked()
{
    if (currentTaskIndex < tasks.size() - 1) {
        currentTaskIndex++;
        setTaskUI();
    }
}

void TaskManager::onPreviousButtonClicked()
{
    if (currentTaskIndex > 0) {
        currentTaskIndex--;
        setTaskUI();
    }
}

void TaskManager::updateButtonStates()
{
    nextButton->setEnabled(currentTaskIndex < tasks.size() - 1);
    previousButton->setEnabled(currentTaskIndex > 0);
}
