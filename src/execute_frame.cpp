#include "learn_environment/execute_frame.hpp"
#include <QDebug>
#include <QSize>

ExecuteFrame::ExecuteFrame(QWidget *parent)
    : QFrame(parent),
      imageLabel(new QLabel(this)),
      textLabel(new QLabel(this)),
      currentMovie(nullptr),
      hLayout(new QHBoxLayout(this))
{
    initializeUI();
}

ExecuteFrame::~ExecuteFrame()
{
    if (currentMovie) {
        currentMovie->stop();
        delete currentMovie;
    }
}

void ExecuteFrame::initializeUI()
{
    // Frame setup
    setStyleSheet("margin: 3 10 6 10; border: 0; border-radius: 6; background:#efefef;");
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum); // Changed to Minimum

    // Layout setup
    hLayout->setSpacing(5);
    hLayout->setContentsMargins(8, 8, 8, 8);

    // Configure imageLabel
    imageLabel->setFixedSize(20, 20);
    imageLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    imageLabel->setStyleSheet("padding: 0px; margin: 0px;");

    // Configure textLabel
    textLabel->setWordWrap(true);
    QFont font = textLabel->font();
    font.setPointSize(10);
    textLabel->setFont(font);
    textLabel->setStyleSheet("margin: 0 0 0 2;");
    textLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    textLabel->setMinimumWidth(200);

    // Add widgets to layout
    hLayout->addWidget(imageLabel, 0, Qt::AlignLeft | Qt::AlignTop);
    hLayout->addWidget(textLabel, 1);
}

void ExecuteFrame::setImage(const QString &path)
{
    if (currentMovie) {
        currentMovie->stop();
        delete currentMovie;
        currentMovie = nullptr;
    }

    if (path.endsWith(".gif", Qt::CaseInsensitive)) {
        currentMovie = new QMovie(path);
        currentMovie->setScaledSize(imageLabel->size());
        if (!currentMovie->isValid()) {
            qDebug() << "Loading GIF failed to load from path:" << path;
            imageLabel->setText("Loading...");
        } else {
            imageLabel->setMovie(currentMovie);
            connect(currentMovie, &QMovie::frameChanged, imageLabel, [this](int) {
                imageLabel->update();
            });
            currentMovie->start();
        }
    } else {
        QPixmap pixmap(path);
        if (pixmap.isNull()) {
            qDebug() << "Failed to load image from path:" << path;
            imageLabel->setText("Image not available");
        } else {
            imageLabel->setPixmap(pixmap.scaled(imageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        }
    }
}

void ExecuteFrame::setText(const QString &text)
{
    textLabel->setText(text);
    textLabel->adjustSize();
    adjustSize();
    updateGeometry();
    layout()->activate(); // Force layout recalculation
}

QString ExecuteFrame::getText()
{
    return textLabel->text();
}