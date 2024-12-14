#include "learn_environment/subtask_item.hpp"
#include "learn_environment/task_manager.hpp"
#include "learn_environment/notebook_converter.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QMenu>
#include <QWidgetAction>
#include <QMessageBox>
#include <QDebug>

namespace {
    const int HEADER_FONT_SIZE = 12;
    const int BODY_FONT_SIZE = 10;
    const char* LINK_STYLE = "font-family:'monospace'; font-size:10pt; color:'#444444'; vertical-align:bottom;";
    const char* BODY_STYLE = "color:#444444;";

    const char* START_ICON_PATH = ":/resource/icons/play.png";
    const char* STOP_ICON_PATH = ":/resource/icons/stop.png";
    const char* HELP_ICON_PATH = ":/resource/icons/help.png";
    const char* LOADING_GIF_PATH = ":/resource/icons/loading.gif";
    const char* QUEUED_GIF_PATH = ":/resource/icons/queued.gif";
    const char* FAILED_ICON_PATH = ":/resource/icons/failed.png";
    const char* SUCCEEDED_ICON_PATH = ":/resource/icons/succeeded.png";

    const char* ICON_BUTTON_STYLE = 
        "QToolButton { border: 0; padding: 0; }"
        "QToolButton:hover { background: #e6e6e6; border-radius: 8px; }";
    const char* MENU_BUTTON_STYLE = 
        "QPushButton { border: 2; padding-left: 25px; padding-right: 25px; padding-top: 5px; padding-bottom: 5px; background: white; border-radius: 8px; }"
        "QPushButton:hover { background: #fafafa; }";

    const char* START_TOOLTIP = "Start Script";
    const char* STOP_TOOLTIP = "Stop Script";
    const char* QUEUED_TOOLTIP = "Queued Execution";
    const char* INACTIVE_TOOLTIP = "Inactive";
    const char* HELP_TOOLTIP = "Help";

    const char* START_OWN_SCRIPT_TEXT = "Start your own Script";
    const char* START_SOLUTION_TEXT = "Start the Solution";

    const char* EXECUTION_SUCCESSFUL_TEXT = "Last Execution completed successfully!";
    const char* EXECUTION_QUEUED_TEXT = "Queued, wait for other processes to finish...";
    const char* EXECUTING_TEXT = "Executing... View logs in terminal.";

    const char* SHOW_SOLUTION_TEXT = "Show Solution";
    const char* HIDE_SOLUTION_TEXT = "Hide Solution";
    const char* RESET_NOTEBOOK_TEXT = "Reset Notebook";

    const char* RESET_NOTEBOOK_CONFIRMATION_TEXT = "This will remove all changes made to the notebook and can't be undone. Are you sure you want to proceed?";
}

SubtaskItem::SubtaskItem(QWidget *parent, Subtask *subtask) 
    : QWidget(parent),
      headerText(subtask->title),
      linkText(subtask->file),
      bodyText(subtask->description),
      subtask(subtask),
      taskManager(nullptr),
      executeSubtaskFrame(nullptr) {
    
    setupItemUI(headerText, linkText, bodyText);
    updateUI(true);
    initializeHelpMenu();
    initializeStartMenu();
}

void SubtaskItem::updateUI(bool constructorCall)
{
    switch (subtask->status) {
        case SubtaskStatus::Inactive:
            startButton->setEnabled(false);
            helpButton->setEnabled(false);
            startButton->setIcon(QIcon(START_ICON_PATH));
            startButton->setToolTip(INACTIVE_TOOLTIP);
            if (!subtask->hasBeenExecuted) {
                break;
            }
            if (subtask->lastExecutionError.isEmpty()) {
                setExecutionFrame(SUCCEEDED_ICON_PATH, EXECUTION_SUCCESSFUL_TEXT);
            } else {
                setExecutionFrame(FAILED_ICON_PATH, subtask->lastExecutionError);
            }
            break;
        case SubtaskStatus::Ready:
            startButton->setEnabled(true);
            helpButton->setEnabled(true);
            startButton->setIcon(QIcon(START_ICON_PATH));
            startButton->setToolTip(START_TOOLTIP);
            if (!subtask->hasBeenExecuted) {
                break;
            }
            if (subtask->lastExecutionError.isEmpty()) {
                setExecutionFrame(SUCCEEDED_ICON_PATH, EXECUTION_SUCCESSFUL_TEXT);
            } else {
                setExecutionFrame(FAILED_ICON_PATH, subtask->lastExecutionError);
            }
            break;
        case SubtaskStatus::Queued:
            startButton->setEnabled(false);
            helpButton->setEnabled(false);
            startButton->setIcon(QIcon(START_ICON_PATH));
            startButton->setToolTip(QUEUED_TOOLTIP);
            setExecutionFrame(QUEUED_GIF_PATH, EXECUTION_QUEUED_TEXT);
            break;
        case SubtaskStatus::Running:
            startButton->setEnabled(true);
            helpButton->setEnabled(false);
            startButton->setIcon(QIcon(STOP_ICON_PATH));
            startButton->setToolTip(STOP_TOOLTIP);
            setExecutionFrame(LOADING_GIF_PATH, EXECUTING_TEXT);
            break;
    }

    if (constructorCall) {
        // menu items are not initialized yet
        return;
    }

    QString notebookPath = subtask->filePath;
    if (!QFile::exists(notebookPath)) {
        return;
    }
    if (NotebookConverter::hasSolutionCells(notebookPath)) {
        menuToggleSolutionBtn->setText(HIDE_SOLUTION_TEXT);
    } else {
        menuToggleSolutionBtn->setText(SHOW_SOLUTION_TEXT);
    }
}

void SubtaskItem::setTaskManager(TaskManager *manager)
{
    taskManager = manager;
}

void SubtaskItem::handleStartButtonClick()
{
    QString notebookPath = subtask->filePath;
    if (NotebookConverter::hasSolutionCells(notebookPath) && subtask->status == SubtaskStatus::Ready) {
        if (startMenu) {
            startMenu->adjustSize();
            QPoint pos = startButton->mapToGlobal(QPoint(startButton->width() - startMenu->width(), startButton->height()));
            startMenu->popup(pos);
        }
    } else {
        if (taskManager && subtask) {
            taskManager->startStopSubtask(*subtask);
        }
    }
}

void SubtaskItem::handleHelpButtonClick()
{
    if (helpMenu) {
        helpMenu->adjustSize();
        QPoint pos = helpButton->mapToGlobal(QPoint(helpButton->width() - helpMenu->width(), helpButton->height()));
        helpMenu->popup(pos);
    }
}


void SubtaskItem::handleStartOwnScript() {
    if (taskManager && subtask) {
        taskManager->startStopSubtask(*subtask);
    }
    QMenu* menu = qobject_cast<QMenu*>(sender()->parent()->parent());
    if (menu) {
        menu->close();
    }
}

void SubtaskItem::handleStartSolution() {
    if (taskManager && subtask) {
        taskManager->startStopSubtask(*subtask, true);
    }
    QMenu* menu = qobject_cast<QMenu*>(sender()->parent()->parent());
    if (menu) {
        menu->close();
    }
}

void SubtaskItem::handleToggleSolution() {
    if (taskManager && subtask) {
        taskManager->toggleSolution(*subtask);
    }
    QMenu* menu = qobject_cast<QMenu*>(sender()->parent()->parent());
    if (menu) {
        menu->close();
    }
}

void SubtaskItem::handleResetNotebook() {
    QMessageBox msgBox;
    msgBox.setText(RESET_NOTEBOOK_CONFIRMATION_TEXT);
    msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Cancel);
    int ret = msgBox.exec();

    if (ret == QMessageBox::Yes) {
        if (taskManager && subtask) {
            NotebookConverter* converter = new NotebookConverter();
            converter->resetNotebook(subtask->filePath, subtask->solutionFilePath);
        }
    }
    QMenu* menu = qobject_cast<QMenu*>(sender()->parent()->parent());
    if (menu) {
        menu->close();
    }
    updateUI();
}

void SubtaskItem::setExecutionFrame(const QString& imagePath, const QString& text) {
    if (!baseLayout) {
        qDebug() << "Error: baseLayout is null";
        return;
    }

    // Only update and return if frame already exists
    if (executeSubtaskFrame) {
        executeSubtaskFrame->setImage(imagePath);
        executeSubtaskFrame->setText(text);
        return;
    }

    // Create new ExecuteFrame
    executeSubtaskFrame = new ExecuteFrame(this);
    if (!executeSubtaskFrame) {
        qDebug() << "Error: Failed to create ExecuteFrame";
        return;
    }

    // Create or retrieve verticalContainer
    QWidget *verticalContainer = new QWidget(this);
    QVBoxLayout *containerLayout = qobject_cast<QVBoxLayout*>(verticalContainer->layout());
    if (!containerLayout) {
        containerLayout = new QVBoxLayout(verticalContainer);
        containerLayout->setContentsMargins(0, 0, 0, 0);
        verticalContainer->setLayout(containerLayout);
    }

    containerLayout->addWidget(executeSubtaskFrame);

    baseLayout->addWidget(verticalContainer, 0);

    // Set initial state
    executeSubtaskFrame->setImage(imagePath);
    executeSubtaskFrame->setText(text);
}


void SubtaskItem::setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText) {
    // Create the main vertical layout
    baseLayout = new QVBoxLayout(this);
    baseLayout->setContentsMargins(0, 0, 0, 0);
    baseLayout->setAlignment(Qt::AlignTop);

    // Create an inner horizontal layout
    QHBoxLayout *innerLayout = new QHBoxLayout();
    innerLayout->setContentsMargins(10, 10, 10, 10);
    innerLayout->setAlignment(Qt::AlignTop);

    // Create a vertical layout for the text
    QVBoxLayout *textLayout = new QVBoxLayout();

    // Create the header label
    QLabel *headerLabel = new QLabel(headerText);
    QFont headerFont = headerLabel->font();
    headerFont.setPointSize(HEADER_FONT_SIZE);
    headerLabel->setFont(headerFont);
    headerLabel->setAlignment(Qt::AlignLeft); // Align to left
    headerLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Create a link label
    QLabel *linkLabel = new QLabel(linkText);
    linkLabel->setTextFormat(Qt::RichText);
    linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setStyleSheet(LINK_STYLE);
    linkLabel->setAlignment(Qt::AlignLeft); // Align to left
    linkLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    // Create the text label with automatic line breaks
    QLabel *loremLabel = new QLabel(bodyText);
    loremLabel->setWordWrap(true);
    loremLabel->setTextFormat(Qt::RichText);
    QFont loremFont = loremLabel->font();
    loremFont.setPointSize(BODY_FONT_SIZE);
    loremLabel->setFont(loremFont);
    loremLabel->setStyleSheet(BODY_STYLE);
    loremLabel->setAlignment(Qt::AlignLeft); // Align to left
    loremLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Minimum);

    // Add header, link, and text labels to the vertical layout
    textLayout->addWidget(headerLabel);
    textLayout->addWidget(linkLabel);
    textLayout->addWidget(loremLabel);

    // Add the text layout to the inner horizontal layout
    innerLayout->addLayout(textLayout, 1);

    // Create a vertical layout for the buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setAlignment(Qt::AlignTop); // Align buttons to the top
    buttonLayout->setContentsMargins(10, 0, 0, 0);

    // Create start button
    startButton = new QToolButton(this);
    startButton->setToolTip(START_TOOLTIP);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setStyleSheet(ICON_BUTTON_STYLE);
    startButton->setIcon(QIcon(START_ICON_PATH));
    startButton->setIconSize(QSize(35, 35));
    startButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Create help button
    helpButton = new QToolButton(this);
    helpButton->setToolTip(HELP_TOOLTIP);
    helpButton->setCursor(Qt::PointingHandCursor);
    helpButton->setStyleSheet(ICON_BUTTON_STYLE);
    helpButton->setIcon(QIcon(HELP_ICON_PATH));
    helpButton->setIconSize(QSize(35, 35));
    helpButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Add buttons to the button layout
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(helpButton);

    // Add the button layout to the inner horizontal layout
    innerLayout->addLayout(buttonLayout);

    // Add the inner horizontal layout to the base vertical layout
    baseLayout->addLayout(innerLayout);

    // Connect the execute button to the popup
    connect(startButton, &QPushButton::clicked, this, &SubtaskItem::handleStartButtonClick);
    connect(helpButton, &QPushButton::clicked, this, &SubtaskItem::handleHelpButtonClick);
}

void SubtaskItem::initializeHelpMenu() {
    helpButton->setPopupMode(QToolButton::InstantPopup);

    QString existingStyle = helpButton->styleSheet();
    helpButton->setStyleSheet(existingStyle + " QToolButton::menu-indicator { image: none; }");

    helpMenu = new QMenu(helpButton);
    helpMenu->setAttribute(Qt::WA_TranslucentBackground);
    helpMenu->setWindowFlags(helpMenu->windowFlags() | Qt::FramelessWindowHint);

    auto menuWidget = new QWidget();
    menuWidget->setStyleSheet("background-color: #efefef; border-radius: 12px;");
    auto menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(10, 10, 10, 10);

    menuToggleSolutionBtn = new QPushButton(SHOW_SOLUTION_TEXT);
    menuResetNotebookBtn = new QPushButton(RESET_NOTEBOOK_TEXT);

    menuToggleSolutionBtn->setStyleSheet(MENU_BUTTON_STYLE);
    menuResetNotebookBtn->setStyleSheet(MENU_BUTTON_STYLE);

    menuToggleSolutionBtn->setCursor(Qt::PointingHandCursor);
    menuResetNotebookBtn->setCursor(Qt::PointingHandCursor);

    menuToggleSolutionBtn->setFlat(true);
    menuResetNotebookBtn->setFlat(true);

    menuLayout->addWidget(menuToggleSolutionBtn);
    menuLayout->addWidget(menuResetNotebookBtn);

    auto menuAction = new QWidgetAction(helpMenu);
    menuAction->setDefaultWidget(menuWidget);

    helpMenu->addAction(menuAction);

    // Reset the button state when the menu hides
    connect(helpMenu, &QMenu::aboutToHide, [this]() {
        helpButton->setDown(false);
        helpButton->update();
    });

    // Connect the menu buttons to their handlers
    connect(menuToggleSolutionBtn, &QPushButton::clicked, this, &SubtaskItem::handleToggleSolution);
    connect(menuResetNotebookBtn, &QPushButton::clicked, this, &SubtaskItem::handleResetNotebook);
}

void SubtaskItem::initializeStartMenu() {
    startButton->setPopupMode(QToolButton::InstantPopup);

    QString existingStyle = startButton->styleSheet();
    startButton->setStyleSheet(existingStyle + " QToolButton::menu-indicator { image: none; }");

    startMenu = new QMenu(startButton);
    startMenu->setAttribute(Qt::WA_TranslucentBackground);
    startMenu->setWindowFlags(startMenu->windowFlags() | Qt::FramelessWindowHint);

    auto menuWidget = new QWidget();
    menuWidget->setStyleSheet("background-color: #efefef; border-radius: 12px;");
    auto menuLayout = new QVBoxLayout(menuWidget);
    menuLayout->setContentsMargins(10, 10, 10, 10);

    menuStartOwnBtn = new QPushButton(START_OWN_SCRIPT_TEXT);
    menuStartSolutionBtn = new QPushButton(START_SOLUTION_TEXT);
    
    menuStartOwnBtn->setStyleSheet(MENU_BUTTON_STYLE);
    menuStartSolutionBtn->setStyleSheet(MENU_BUTTON_STYLE);

    menuStartOwnBtn->setCursor(Qt::PointingHandCursor);
    menuStartSolutionBtn->setCursor(Qt::PointingHandCursor);

    menuStartOwnBtn->setFlat(true);
    menuStartSolutionBtn->setFlat(true);

    menuLayout->addWidget(menuStartOwnBtn);
    menuLayout->addWidget(menuStartSolutionBtn);

    auto menuAction = new QWidgetAction(startMenu);
    menuAction->setDefaultWidget(menuWidget);

    startMenu->addAction(menuAction);

    // Reset the button state when the menu hides
    connect(startMenu, &QMenu::aboutToHide, [this]() {
        startButton->setDown(false);
        startButton->update();
    });

    // Connect the new menu buttons to their handlers
    connect(menuStartOwnBtn, &QPushButton::clicked, this, &SubtaskItem::handleStartOwnScript);
    connect(menuStartSolutionBtn, &QPushButton::clicked, this, &SubtaskItem::handleStartSolution);
}