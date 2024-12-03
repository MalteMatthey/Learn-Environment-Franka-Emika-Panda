#ifndef CUSTOMLISTWIDGET_H
#define CUSTOMLISTWIDGET_H

#include <QListWidget>
#include <QMouseEvent>
#include <QApplication>

class CustomListWidget : public QListWidget {
    Q_OBJECT

public:
    explicit CustomListWidget(QWidget *parent = nullptr);
    void formatListWidget();

protected:
    void mouseMoveEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;
};

#endif // CUSTOMLISTWIDGET_H