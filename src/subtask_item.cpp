// SUBTASK_ITEM.CPP
#include "learn_environment/subtask_item.hpp"
#include "learn_environment/task_manager.hpp"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

namespace {
    const int HEADER_FONT_SIZE = 14;
    const int BODY_FONT_SIZE = 10;
    const char* LINK_STYLE = "font-family:'monospace'; font-size:10pt; color:'#444444'; vertical-align:bottom;";
    const char* BODY_STYLE = "color:#444444;";

    const char* START_TEXT = "Start";
    const char* START_TOOLTIP = "Start Script";
    const char* STOP_TEXT = "Stop";
    const char* STOP_TOOLTIP = "Stop Script";
    const char* QUEUED_TEXT = "Queued";
    const char* QUEUED_TOOLTIP = "Queued Execution";
    const char* RESET_TEXT = "Reset";
    const char* RESET_TOOLTIP = "Reset";
    const char* SOLUTION_TEXT = "Solution";
    const char* SOLUTION_TOOLTIP = "Show Solution";
}

SubtaskItem::SubtaskItem(QWidget *parent, Subtask *subtask) 
    : QWidget(parent),
      headerText(subtask->title),
      linkText(subtask->file),
      bodyText(subtask->description),
      subtask(subtask),
      taskManager(nullptr) {
    
    setupItemUI(headerText, linkText, bodyText);
    updateUI();
}

void SubtaskItem::updateUI()
{
    switch (subtask->status) {
        case SubtaskStatus::Inactive:
            playButton->setEnabled(false);
            resetButton->setEnabled(false);
            solutionButton->setEnabled(false);
            playButton->setText(START_TEXT);
            playButton->setToolTip(START_TOOLTIP);
            break;
        case SubtaskStatus::Ready:
            playButton->setEnabled(true);
            resetButton->setEnabled(true);
            solutionButton->setEnabled(true);
            playButton->setText(START_TEXT);
            playButton->setToolTip(START_TOOLTIP);
            break;
        case SubtaskStatus::Queued:
            playButton->setEnabled(false);
            resetButton->setEnabled(false);
            solutionButton->setEnabled(false);
            playButton->setText(QUEUED_TEXT);
            playButton->setToolTip(QUEUED_TOOLTIP);
            break;
        case SubtaskStatus::Running:
            playButton->setEnabled(true);
            resetButton->setEnabled(false);
            solutionButton->setEnabled(false);
            playButton->setText(STOP_TEXT);
            playButton->setToolTip(STOP_TOOLTIP);
            break;
    }
}

void SubtaskItem::setTaskManager(TaskManager *manager)
{
    taskManager = manager;
}

void SubtaskItem::handleStartButtonClick()
{
    if (taskManager && subtask) {
        taskManager->startStopSubtask(*subtask);
    }
}

void SubtaskItem::handleSolutionButtonClick()
{
    if (taskManager && subtask) {
        taskManager->toggleSolution(*subtask);
    }
}

void SubtaskItem::setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText) {
// Create the main widget
    QHBoxLayout *baseLayout = new QHBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 10, 10);

    // Create a vertical layout for the text
    QVBoxLayout *textLayout = new QVBoxLayout();

    // Create the header label
    QLabel *headerLabel = new QLabel(headerText);
    QFont headerFont = headerLabel->font();
    headerFont.setPointSize(HEADER_FONT_SIZE);
    headerLabel->setFont(headerFont);

    // Create a link label
    QLabel *linkLabel = new QLabel(linkText);
    linkLabel->setTextFormat(Qt::RichText);
    linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setStyleSheet(LINK_STYLE);

    // Create the text label with automatic line breaks
    QLabel *loremLabel = new QLabel(bodyText);
    loremLabel->setWordWrap(true); // Enable word wrap
    loremLabel->setTextFormat(Qt::RichText);
    QFont loremFont = loremLabel->font();
    loremFont.setPointSize(BODY_FONT_SIZE);
    loremLabel->setFont(loremFont);
    loremLabel->setStyleSheet(BODY_STYLE);

    // Add header, link, and text labels to the vertical layout
    textLayout->addWidget(headerLabel);
    textLayout->addWidget(linkLabel);
    textLayout->addWidget(loremLabel);
    textLayout->addStretch();

    // Add the text layout to the base layout
    baseLayout->addLayout(textLayout, 1);

    // Create a vertical layout for the buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setAlignment(Qt::AlignRight);
    buttonLayout->setContentsMargins(10, 0, 0, 0);

    // Create buttons
    playButton = new QPushButton(START_TEXT);
    playButton->setToolTip(START_TOOLTIP);
    playButton->setCursor(Qt::PointingHandCursor);

    resetButton = new QPushButton(RESET_TEXT);
    resetButton->setToolTip(RESET_TOOLTIP);
    resetButton->setCursor(Qt::PointingHandCursor);

    solutionButton = new QPushButton(SOLUTION_TEXT);
    solutionButton->setToolTip(SOLUTION_TOOLTIP);
    solutionButton->setCursor(Qt::PointingHandCursor);

    // Add buttons to the button layout
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(solutionButton);

    // Add the button layout to the base layout
    baseLayout->addLayout(buttonLayout);

    // Connect the execute button to the popup
    connect(playButton, &QPushButton::clicked, this, &SubtaskItem::handleStartButtonClick);
    // connect(resetButton, &QPushButton::clicked, this, &SubtaskItem::handleResetButtonClick);
    connect(solutionButton, &QPushButton::clicked, this, &SubtaskItem::handleSolutionButtonClick);
}