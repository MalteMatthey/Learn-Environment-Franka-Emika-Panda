#include "learn_environment/subtask_item.h"
#include <QLabel>
#include <QFrame>

SubtaskItem::SubtaskItem(QWidget *parent, const QString &headerText, const QString &linkText, const QString &bodyText)
    : QWidget(parent) {
    setupItemUI(headerText, linkText, bodyText);
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
    QPushButton *playButton = new QPushButton("Execute");
    playButton->setToolTip("Execute Script");
    playButton->setCursor(Qt::PointingHandCursor);

    QPushButton *resetButton = new QPushButton("Reset");
    resetButton->setToolTip("Reset");
    resetButton->setCursor(Qt::PointingHandCursor);

    QPushButton *solutionButton = new QPushButton("Solution");
    solutionButton->setToolTip("Show Solution");
    solutionButton->setCursor(Qt::PointingHandCursor);

    // Add buttons to the button layout
    buttonLayout->addWidget(playButton);
    buttonLayout->addWidget(resetButton);
    buttonLayout->addWidget(solutionButton);

    // Add the button layout to the base layout
    baseLayout->addLayout(buttonLayout);

    // Connect the execute button to the popup
    connect(playButton, &QPushButton::clicked, this, &SubtaskItem::handleButtonClick);
}

void SubtaskItem::handleButtonClick() {
    QWidget *popup = new QWidget();
    popup->setWindowTitle("Popup");

    QHBoxLayout *popupLayout = new QHBoxLayout(popup);
    QLabel *popupLabel = new QLabel("Sample Text", popup);
    QPushButton *sampleButton = new QPushButton("Sample Button", popup);

    popupLayout->addWidget(popupLabel);
    popupLayout->addWidget(sampleButton);

    popup->setLayout(popupLayout);
    popup->show();
}