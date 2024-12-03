#include "learn_environment/custom_list_widget.h"

CustomListWidget::CustomListWidget(QWidget *parent)
    : QListWidget(parent) {
    setMouseTracking(true);
    formatListWidget();
}

void CustomListWidget::formatListWidget() {
    setStyleSheet("background: transparent;");
    setSelectionMode(QAbstractItemView::SingleSelection);
    setFocusPolicy(Qt::StrongFocus);
    setAttribute(Qt::WA_TranslucentBackground);
}

void CustomListWidget::mouseMoveEvent(QMouseEvent *event) {
    QListWidgetItem *item = itemAt(event->pos());
    // Check if the item is valid and not a header or margin item
    if (item && item->flags() != Qt::NoItemFlags) {
        setCursor(Qt::PointingHandCursor);
    } else {
        setCursor(Qt::ArrowCursor);
    }
    QListWidget::mouseMoveEvent(event);
}

void CustomListWidget::leaveEvent(QEvent *event) {
    // Reset the cursor to the default arrow cursor when the mouse leaves the widget
    unsetCursor();
    QListWidget::leaveEvent(event);
}