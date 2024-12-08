// SUBTASK_ITEM.CPP
#include "learn_environment/subtask_item.h"
#include "learn_environment/task_manager.h"

#include <QPushButton>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

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
            playButton->setText("Start");
            playButton->setToolTip("Start Script");
            break;
        case SubtaskStatus::Ready:
            playButton->setEnabled(true);
            resetButton->setEnabled(true);
            solutionButton->setEnabled(true);
            playButton->setText("Start");
            playButton->setToolTip("Start Script");
            break;
        case SubtaskStatus::Queued:
            playButton->setEnabled(false);
            resetButton->setEnabled(false);
            solutionButton->setEnabled(false);
            playButton->setText("Queued");
            playButton->setToolTip("Queued Execution");
            break;
        case SubtaskStatus::Running:
            playButton->setEnabled(true);
            resetButton->setEnabled(false);
            solutionButton->setEnabled(false);
            playButton->setText("Stop");
            playButton->setToolTip("Stop Script");
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

void SubtaskItem::setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText) {
    // Create the main widget
    QHBoxLayout *baseLayout = new QHBoxLayout(this);
    baseLayout->setContentsMargins(10, 10, 10, 10);

    // Create a vertical layout for the text
    QVBoxLayout *textLayout = new QVBoxLayout();

    // Create a horizontal layout for the header and link
    QHBoxLayout *headerLinkLayout = new QHBoxLayout();

    // Create a header label
    QLabel *headerLabel = new QLabel(headerText);
    QFont headerFont = headerLabel->font();
    headerFont.setPointSize(14);
    headerLabel->setFont(headerFont);

    // Create a link label
    QLabel *linkLabel = new QLabel(linkText);
    linkLabel->setTextFormat(Qt::RichText);
    linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
    linkLabel->setOpenExternalLinks(true);
    linkLabel->setStyleSheet("font-family:'monospace'; font-size:10pt; color:'#444444'; vertical-align:bottom;");

    // Add header and link labels to the horizontal layout
    headerLinkLayout->addWidget(headerLabel);
    headerLinkLayout->addWidget(linkLabel);
    headerLinkLayout->addStretch();
    headerLinkLayout->setAlignment(linkLabel, Qt::AlignBottom);

    // Create the text label with automatic line breaks
    QLabel *loremLabel = new QLabel(bodyText);
    loremLabel->setWordWrap(true); // Enable word wrap
    loremLabel->setTextFormat(Qt::RichText);
    QFont loremFont = loremLabel->font();
    loremFont.setPointSize(10);
    loremLabel->setFont(loremFont);
    loremLabel->setStyleSheet("color:#444444;");

    // Add headerLinkLayout and loremLabel to the vertical layout
    textLayout->addLayout(headerLinkLayout);
    textLayout->addWidget(loremLabel);
    textLayout->addStretch();

    // Add the text layout to the base layout
    baseLayout->addLayout(textLayout, 1);

    // Create a vertical layout for the buttons
    QVBoxLayout *buttonLayout = new QVBoxLayout();
    buttonLayout->setAlignment(Qt::AlignRight);
    buttonLayout->setContentsMargins(10, 0, 0, 0); 

    // Create buttons
    playButton = new QPushButton("Start");
    playButton->setToolTip("Start Script");
    playButton->setCursor(Qt::PointingHandCursor);

    resetButton = new QPushButton("Reset");
    resetButton->setToolTip("Reset");
    resetButton->setCursor(Qt::PointingHandCursor);

    solutionButton = new QPushButton("Solution");
    solutionButton->setToolTip("Show Solution");
    solutionButton->setCursor(Qt::PointingHandCursor);

    // Add buttons to the button layout
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(solutionButton);

    // Add the button layout to the base layout
    baseLayout->addLayout(buttonLayout);

    // Connect the execute button to the popup
    connect(playButton, &QPushButton::clicked, this, &SubtaskItem::handleStartButtonClick);
}
