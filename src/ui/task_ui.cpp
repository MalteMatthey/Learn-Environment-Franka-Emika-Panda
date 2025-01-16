#include "ui/task_ui.hpp"
#include "ui/subtask_item.hpp"

#include <QRegExp>
#include <QSplitter>
#include <QDebug>
#include <QHBoxLayout>

namespace {
    const char* FOLDER_HTML_TEMPLATE = R"(
        <!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd">
        <body style=" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;">
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px; line-height:60%;">Folder for this task:</p>
        <p style=" margin-top:12px; margin-bottom:12px; margin-left:0px; margin-right:0px; 
            -qt-block-indent:0; text-indent:0px;"><span style="font-family:'monospace'; 
            color:'#444444';">)";
    const char* CLOSE_ICON_PATH = ":/icons/close.png";
    const char* MENU_ICON_PATH = ":/icons/menu.png";
    const char* LOADING_GIF_PATH = ":/icons/loading.gif";
    const char* START_ICON_PATH = ":/icons/play.png";
    const char* STOP_ICON_PATH = ":/icons/stop.png";
    const char* FAILED_ICON_PATH = ":/icons/failed.png";
    const char* SUCCEEDED_ICON_PATH = ":/icons/succeeded.png";
    const char* RESET_ROBOT_TEXT = "Resetting the robot to its default state...";
    const char* RESET_ROBOT_TEXT_SCRIPTS = "Resetting the robot before script execution...";
    const char* RESET_ROBOT_TEXT_SUCCESS = "Robot successfully reset!";
    const char* RESET_ROBOT_TEXT_FAILED = "<b>Robot reset failed!</b>";
    const char* STOP_RESET_ROBOT_TOOLTIP = "Cancel reset process";
    const char* RESET_ROBOT_TOOLTIP = "Reset robot to default state";
}

TaskUI::TaskUI(QVBoxLayout *subtaskListLayout, QLabel *mainTitleLabel,
               QLabel *difficultyLabel, QLabel *folderLabel, QLabel *topicLabel,
               QPushButton *nextButton, QPushButton *previousButton, QToolButton *menuButton,
               QToolButton *resetRobotStartButton, QFrame *resetRobotFrame, QWidget *centralwidget, QWidget *parent)
    : QWidget(parent),
      sidebar(new Sidebar(this)),
      subtaskListLayout(subtaskListLayout),
      mainTitleLabel(mainTitleLabel),
      difficultyLabel(difficultyLabel),
      folderLabel(folderLabel),
      topicLabel(topicLabel),
      nextButton(nextButton),
      previousButton(previousButton),
      menuButton(menuButton),
      resetRobotStartButton(resetRobotStartButton),
      resetRobotFrame(resetRobotFrame),
      executeResetRobotFrame(nullptr),
      centralwidget(centralwidget)
{ }

void TaskUI::setTaskManager(TaskManager *manager)
{
    taskManager = manager;
    connect(resetRobotStartButton, &QToolButton::clicked, taskManager, &TaskManager::forceResetRobot);
    connect(nextButton, &QPushButton::clicked, taskManager, &TaskManager::nextTask);
    connect(previousButton, &QPushButton::clicked, taskManager, &TaskManager::previousTask);
    connect(this, &TaskUI::taskSelected, taskManager, &TaskManager::selectTask);
}

void TaskUI::initializeUI(const QVector<QSharedPointer<Task>> &loadedTasks)
{
    tasks = loadedTasks;
    if (tasks.isEmpty()) {
        qCritical() << "No tasks loaded. Exiting TaskUI initialization.";
        return;
    }

    sidebar->fillSidebarWithTasks(tasks);
    sidebar->setVisible(false);

    setupSplitterAndLayout();

    connect(menuButton, &QPushButton::clicked, this, &TaskUI::toggleSidebarVisibility);
    connect(sidebar, &Sidebar::taskSelected, this, &TaskUI::taskSelected);
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

    sidebar->selectTask(currentTaskIndex);
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

void TaskUI::setupSplitterAndLayout() {
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    centralwidget->setStyleSheet("border: 0; margin: 0;");

    splitter->addWidget(sidebar);
    splitter->addWidget(centralwidget);
    splitter->setStyleSheet("border: 0; margin: 0;");
    splitter->setContentsMargins(0, 0, 0, 0);

    QList<int> sizes;
    sizes << 200 << 600;
    splitter->setSizes(sizes);

    // Set the layout to the main widget
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(splitter);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainLayout);
}

void TaskUI::toggleSidebarVisibility() {
    bool isVisible = sidebar->isVisible();
    sidebar->setVisible(!isVisible);

    if (sidebar->isVisible()) {
        menuButton->setIcon(QIcon(CLOSE_ICON_PATH));
    } else {
        menuButton->setIcon(QIcon(MENU_ICON_PATH));
    }
}

void TaskUI::startedRobotResetUI(bool noSubtasksLeft) {
    resetRobotStartButton->setIcon(QIcon(STOP_ICON_PATH));
    resetRobotStartButton->setToolTip(STOP_RESET_ROBOT_TOOLTIP);


    if (!executeResetRobotFrame) {
        executeResetRobotFrame = new ExecuteFrame(resetRobotFrame);
        QVBoxLayout *resetLayout = qobject_cast<QVBoxLayout*>(resetRobotFrame->layout());
        if (!resetLayout) {
            resetLayout = new QVBoxLayout(resetRobotFrame);
            resetRobotFrame->setLayout(resetLayout);
        }
        if (!resetLayout) {
            qCritical() << "Reset layout not found.";
            return;
        }
        resetLayout->addWidget(executeResetRobotFrame);
    }
    if (!executeResetRobotFrame) {
        qCritical() << "Execute reset robot frame not found.";
        return;
    }
    executeResetRobotFrame->setImage(LOADING_GIF_PATH);
    if (noSubtasksLeft) {
        executeResetRobotFrame->setText(RESET_ROBOT_TEXT);
    } else {
        executeResetRobotFrame->setText(RESET_ROBOT_TEXT_SCRIPTS);
    }
}

void TaskUI::finishedRobotResetUI() {
    resetRobotStartButton->setIcon(QIcon(START_ICON_PATH));
    resetRobotStartButton->setToolTip(RESET_ROBOT_TOOLTIP);
    if (executeResetRobotFrame) {
        if (!executeResetRobotFrame->getText().contains(RESET_ROBOT_TEXT_FAILED)) {
            executeResetRobotFrame->setImage(SUCCEEDED_ICON_PATH);
            executeResetRobotFrame->setText(RESET_ROBOT_TEXT_SUCCESS);
        }
    }
}

void TaskUI::failedRobotResetUI(const QString &error) {
    if (executeResetRobotFrame) {
        executeResetRobotFrame->setImage(FAILED_ICON_PATH);
        executeResetRobotFrame->setText(QString(RESET_ROBOT_TEXT_FAILED) + "<br>" + error);
    }
}