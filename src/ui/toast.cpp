#include "ui/toast.hpp"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QVBoxLayout>

Toast::Toast(QWidget *parent) : QWidget(parent), toastLabel(new QLabel(this)), toastTimer(new QTimer(this)) {
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(toastLabel);
    layout->setContentsMargins(0, 0, 0, 0);
    
    toastLabel->setStyleSheet("background-color: rgba(0, 0, 0, 160); color: white; padding: 6px; border-radius: 6px;");
    toastLabel->setAlignment(Qt::AlignCenter);
    toastLabel->setWordWrap(true);
    QFont toastFont = toastLabel->font();
    toastFont.setPointSize(10);
    toastLabel->setFont(toastFont);
    toastLabel->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    toastLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
    toastLabel->hide();

    connect(toastTimer, &QTimer::timeout, this, &Toast::fadeOut);
}

void Toast::showToast(const QString &message) {
    toastLabel->setText(message);
    toastLabel->adjustSize();

    this->resize(toastLabel->sizeHint());
    toastLabel->move(0, 0);

    int margin = 10;
    int x = (parentWidget()->width() - this->width()) / 2;
    int y = parentWidget()->height() - this->height() - margin;
    this->move(x, y);

    QGraphicsEffect* existingEffect = toastLabel->graphicsEffect();
    if (existingEffect) {
        delete existingEffect;
        toastLabel->setGraphicsEffect(nullptr);
    }

    QGraphicsOpacityEffect* opacityEffect = new QGraphicsOpacityEffect(toastLabel);
    toastLabel->setGraphicsEffect(opacityEffect);

    QPropertyAnimation* fadeIn = new QPropertyAnimation(opacityEffect, "opacity");
    fadeIn->setDuration(500);
    fadeIn->setStartValue(0);
    fadeIn->setEndValue(1);
    fadeIn->setEasingCurve(QEasingCurve::InOutQuad);
    fadeIn->start(QPropertyAnimation::DeleteWhenStopped);

    this->raise();
    toastLabel->show();
    this->show();
    toastTimer->start(2500);
}

void Toast::fadeOut() {
    if (!toastLabel || !toastLabel->graphicsEffect() || toastLabel->isHidden())
        return;

    QGraphicsOpacityEffect* effect = qobject_cast<QGraphicsOpacityEffect*>(toastLabel->graphicsEffect());
    if (!effect) return;

    QPropertyAnimation* fadeOut = new QPropertyAnimation(effect, "opacity");
    fadeOut->setDuration(500);
    fadeOut->setStartValue(1);
    fadeOut->setEndValue(0);
    fadeOut->setEasingCurve(QEasingCurve::InOutQuad);
    connect(fadeOut, &QPropertyAnimation::finished, this, [this, effect]() {
        toastLabel->hide();
        toastLabel->setGraphicsEffect(nullptr);
    });
    fadeOut->start(QPropertyAnimation::DeleteWhenStopped);
}