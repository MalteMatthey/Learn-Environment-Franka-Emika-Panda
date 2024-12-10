#include "learn_environment/sidebar.hpp"
#include "learn_environment/custom_list_widget.hpp"
#include "learn_environment/task.hpp"

#include <QFont>
#include <QDebug>
#include <QBrush>
#include <QMap>
#include <QStringList>
#include <QObject>
#include <QVBoxLayout>
#include <QListWidgetItem>

namespace {
    const int SIDEBAR_MINIMUM_WIDTH = 100;
    const int SIDEBAR_TITLE_FONT_SIZE = 20;
    const char* SIDEBAR_TITLE_LABEL = "TASKS";

    const QSize SIDEBAR_ITEM_MARGIN_SIZE(0, 30);
    const int SIDEBAR_ITEM_FONT_SIZE = 12;

    const char* BEGINNER_JSON = "beginner";
    const char* INTERMEDIATE_JSON = "intermediate";
    const char* ADVANCED_JSON = "advanced";
    const char* UNKNOWN_JSON = "unknown";

    const QStringList DIFFICULTY_ORDER = {BEGINNER_JSON, INTERMEDIATE_JSON, ADVANCED_JSON, UNKNOWN_JSON};

    QString capitalize(const QString& str) {
        if (str.isEmpty()) return str;
        return str.at(0).toUpper() + str.mid(1);
    }
}

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    listWidget = createList();
    setMinimumWidth(SIDEBAR_MINIMUM_WIDTH);

    // Connect the itemClicked signal to the slot
    connect(listWidget, &QListWidget::itemClicked, this, &Sidebar::onTaskItemClicked);
}

CustomListWidget* Sidebar::createList() {
    QVBoxLayout *sidebarLayout = new QVBoxLayout(this);

    QLabel *sidebarTitle = new QLabel(tr(SIDEBAR_TITLE_LABEL));
    sidebarTitle->setFont(QFont("", SIDEBAR_TITLE_FONT_SIZE));
    sidebarLayout->addWidget(sidebarTitle);

    CustomListWidget *taskListWidget = new CustomListWidget();
    taskListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sidebarLayout->addWidget(taskListWidget);

    this->setLayout(sidebarLayout);

    return taskListWidget;
}

void Sidebar::fillSidebarWithTasks(const QVector<QSharedPointer<Task>>& tasks) {
    // Create fonts for headers and normal text
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPointSize(SIDEBAR_ITEM_FONT_SIZE);
    QFont normalFont;
    normalFont.setPointSize(SIDEBAR_ITEM_FONT_SIZE);

    // Clear existing items and the index-to-item map
    listWidget->clear();
    taskIndexToItemMap.clear();

    // Map to group tasks by difficulty
    QMap<QString, QList<QPair<int, QSharedPointer<Task>>>> tasksByDifficulty;

    // Populate the tasksByDifficulty map
    for (int i = 0; i < static_cast<int>(tasks.size()); ++i) {
        QSharedPointer<Task> task = tasks[i];
        QString difficultyKey = task->difficulty.toLower();
        if (difficultyKey != BEGINNER_JSON && difficultyKey != INTERMEDIATE_JSON && difficultyKey != ADVANCED_JSON) {
            difficultyKey = UNKNOWN_JSON;
        }
        tasksByDifficulty[difficultyKey].append(qMakePair(i, task));
    }

    // Add tasks to the list widget in the desired order
    for (const QString &difficultyKey : DIFFICULTY_ORDER) {
        if (tasksByDifficulty.contains(difficultyKey)) {
            // Add header
            QListWidgetItem *headerItem = new QListWidgetItem(capitalize(difficultyKey));
            headerItem->setFont(boldFont);
            headerItem->setSizeHint(SIDEBAR_ITEM_MARGIN_SIZE);
            headerItem->setFlags(Qt::NoItemFlags);
            headerItem->setForeground(QBrush(Qt::black));
            listWidget->addItem(headerItem);

            // Add tasks under the header
            const QList<QPair<int, QSharedPointer<Task>>> &tasksInSection = tasksByDifficulty[difficultyKey];
            for (const QPair<int, QSharedPointer<Task>> &pair : tasksInSection) {
                int taskIndex = pair.first;
                QSharedPointer<Task> task = pair.second;

                QListWidgetItem *taskItem = new QListWidgetItem(task->title);
                taskItem->setFont(normalFont);
                taskItem->setSizeHint(SIDEBAR_ITEM_MARGIN_SIZE);

                // Store the original index of the task
                taskItem->setData(Qt::UserRole, taskIndex);
                listWidget->addItem(taskItem);

                // Map task index to QListWidgetItem
                taskIndexToItemMap[taskIndex] = taskItem;
            }

            // Add spacer after tasks
            QListWidgetItem *marginItem = new QListWidgetItem("");
            marginItem->setSizeHint(SIDEBAR_ITEM_MARGIN_SIZE);
            marginItem->setFlags(Qt::NoItemFlags);
            listWidget->addItem(marginItem);
        }
    }
}

void Sidebar::onTaskItemClicked(QListWidgetItem *item)
{
    // Check if the item has a valid task index
    if (item->data(Qt::UserRole).isValid()) {
        int taskIndex = item->data(Qt::UserRole).toInt();
        Q_EMIT taskSelected(taskIndex);
    }
}

void Sidebar::selectTask(int taskIndex) {
    // Check if the task index exists in the map
    if (taskIndexToItemMap.contains(taskIndex)) {
        QListWidgetItem *item = taskIndexToItemMap[taskIndex];
        // Clear the current selection
        listWidget->clearSelection();
        // Select the matching item
        item->setSelected(true);
        listWidget->scrollToItem(item);
    } else {
        qWarning() << "Task with index" << taskIndex << "not found in the list.";
    }
}