#ifndef SIDEBAR_H
#define SIDEBAR_H

#include "custom_list_widget.h"
#include "task.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QListWidget>
#include <QSharedPointer>

/**
 * @class Sidebar
 * @brief Represents the sidebar UI component for displaying tasks.
 *
 * The Sidebar class is responsible for displaying a list of tasks in a sidebar.
 * It provides functionalities to populate the sidebar with tasks, handle user interactions,
 * and select tasks programmatically.
 */
class Sidebar : public QWidget
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a Sidebar object.
     * @param parent Pointer to the parent widget.
     */
    explicit Sidebar(QWidget *parent = nullptr);

    /**
     * @brief Populates the sidebar with tasks.
     * @param tasks Vector of shared pointers to the tasks to be displayed.
     */
    void fillSidebarWithTasks(const QVector<QSharedPointer<Task>>& tasks);

    /**
     * @brief Selects a task in the sidebar by its index.
     * @param index The index of the task to be selected.
     */
    void selectTask(int index);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a task is selected.
     * @param index The index of the selected task.
     */
    void taskSelected(int index);

private Q_SLOTS:
    /**
     * @brief Handles the task item click event.
     * @param item Pointer to the clicked QListWidgetItem.
     */
    void onTaskItemClicked(QListWidgetItem *item);

private:
    /**
     * @brief Creates the list widget for displaying tasks.
     * @return Pointer to the created CustomListWidget.
     */
    CustomListWidget* createList();

    CustomListWidget *listWidget; ///< Pointer to the CustomListWidget for displaying tasks.
    QMap<int, QListWidgetItem*> taskIndexToItemMap; ///< Map from task index to QListWidgetItem.
};

#endif // SIDEBAR_H