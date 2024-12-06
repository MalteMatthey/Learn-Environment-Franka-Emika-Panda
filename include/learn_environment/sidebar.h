#ifndef SIDEBAR_H
#define SIDEBAR_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QListWidget>
#include "custom_list_widget.h"
#include "task.h"
#include <QSharedPointer>

class Sidebar : public QWidget
{
    Q_OBJECT

public:
    explicit Sidebar(QWidget *parent = nullptr);
    void fillSidebarWithTasks(const QVector<QSharedPointer<Task>>& tasks);
    void selectTask(int index);

Q_SIGNALS:
    void taskSelected(int index);

private Q_SLOTS:
    void onTaskItemClicked(QListWidgetItem *item);

private:
    CustomListWidget* createList();
    CustomListWidget *listWidget;
    QMap<int, QListWidgetItem*> taskIndexToItemMap;
};

#endif // SIDEBAR_H