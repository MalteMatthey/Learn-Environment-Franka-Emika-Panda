#include "learn_environment/custom_list_widget.hpp"

CustomListWidget::CustomListWidget(QWidget *parent)
    : QListWidget(parent) {
    setMouseTracking(true);
    formatListWidget();
}

void CustomListWidget::formatListWidget() {
    setStyleSheet("QListView {background: transparent}"
                "QListView::item {padding: 0 6px; border-radius: 8px;}"
                "QListView::item:hover {background: white;}"
                "QListView::item:selected {background: #284387; border-radius: 8px;}"
                "QListView::item:selected:hover {background: #465e9c; border-radius: 8px;}"
                "QScrollBar:horizontal {border: none; background: none; height: 24px; margin: 3px 3px 3px 3px;}"
                "QScrollBar::handle:horizontal {background: lightgray; min-width: 16px; margin: 3px 3px; border-radius: 4px;}"
                "QScrollBar::add-line:horizontal {background: none; width: 16px; subcontrol-position: right; subcontrol-origin: margin;}"
                "QScrollBar::sub-line:horizontal {background: none; width: 16px; subcontrol-position: top left; subcontrol-origin: margin; position: absolute;}"
                "QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {background: none;}"
                "QScrollBar:vertical {border: none; background: none; width: 24px; margin: 3px 3px 3px 3px;}"
                "QScrollBar::handle:vertical {background: lightgray; min-height: 16px; margin: 3px 3px; border-radius: 4px;}"
                "QScrollBar::add-line:vertical {background: none; height: 16px; subcontrol-position: bottom; subcontrol-origin: margin;}"
                "QScrollBar::sub-line:vertical {background: none; height: 16px; subcontrol-position: top left; subcontrol-origin: margin; position: absolute;}"
                "QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {background: none;}");
    
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