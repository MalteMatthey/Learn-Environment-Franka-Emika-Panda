#include "learn_environment/sidebar.h"
#include "learn_environment/custom_list_widget.h"
#include "learn_environment/task.h"
#include <QFont>
#include <QDebug>
#include <QBrush>
#include <QMap>
#include <QStringList>
#include <QObject> 

Sidebar::Sidebar(QWidget *parent)
    : QWidget(parent)
{
    listWidget = createList();
    setMinimumWidth(100);

    // Connect the itemClicked signal to the slot
    connect(listWidget, &QListWidget::itemClicked, this, &Sidebar::onTaskItemClicked);
}

CustomListWidget* Sidebar::createList() {
    QVBoxLayout *sidebarLayout = new QVBoxLayout(this);

    QLabel *sidebarTitle = new QLabel(tr("TASKS"));
    sidebarTitle->setFont(QFont("", 20));
    sidebarLayout->addWidget(sidebarTitle);

    CustomListWidget *taskListWidget = new CustomListWidget();
    taskListWidget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    sidebarLayout->addWidget(taskListWidget);

    this->setLayout(sidebarLayout);

    return taskListWidget;
}

void Sidebar::fillSidebarWithTasks(const std::vector<Task>& tasks) {
    // Create fonts for headers and normal text
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPointSize(12);
    QFont normalFont;
    normalFont.setPointSize(12);

    // Define margin size for spacing
    QSize marginSize(0, 30);

    // Clear existing items and the index-to-item map
    listWidget->clear();
    taskIndexToItemMap.clear();

    // Map to group tasks by difficulty
    QMap<QString, QList<QPair<int, Task>>> tasksByDifficulty;

    // Populate the tasksByDifficulty map
    for (int i = 0; i < static_cast<int>(tasks.size()); ++i) {
        const Task &task = tasks[i];
        QString difficultyKey = QString::fromStdString(task.difficulty).toLower();
        if (difficultyKey != "beginner" && difficultyKey != "intermediate" && difficultyKey != "advanced") {
            difficultyKey = "unknown";
        }
        tasksByDifficulty[difficultyKey].append(qMakePair(i, task));
    }

    // Define the order of difficulties
    QStringList difficultyOrder = {"beginner", "intermediate", "advanced", "unknown"};

    // Map to display names for difficulties
    QMap<QString, QString> difficultyLabels = {
        {"beginner", "Beginner"},
        {"intermediate", "Intermediate"},
        {"advanced", "Advanced"},
        {"unknown", "Unknown"}
    };

    // Add tasks to the list widget in the desired order
    for (const QString &difficultyKey : difficultyOrder) {
        if (tasksByDifficulty.contains(difficultyKey)) {
            // Add header
            QListWidgetItem *headerItem = new QListWidgetItem(difficultyLabels[difficultyKey]);
            headerItem->setFont(boldFont);
            headerItem->setSizeHint(marginSize);
            headerItem->setFlags(Qt::NoItemFlags);
            headerItem->setForeground(QBrush(Qt::black));
            listWidget->addItem(headerItem);

            // Add tasks under the header
            const QList<QPair<int, Task>> &tasksInSection = tasksByDifficulty[difficultyKey];
            for (const QPair<int, Task> &pair : tasksInSection) {
                int taskIndex = pair.first;
                const Task &task = pair.second;

                QListWidgetItem *taskItem = new QListWidgetItem(QString::fromStdString(task.title));
                taskItem->setFont(normalFont);
                taskItem->setSizeHint(marginSize);

                // Store the original index of the task
                taskItem->setData(Qt::UserRole, taskIndex);
                listWidget->addItem(taskItem);

                // Map task index to QListWidgetItem
                taskIndexToItemMap[taskIndex] = taskItem;
            }

            // Add spacer after tasks
            QListWidgetItem *marginItem = new QListWidgetItem("");
            marginItem->setSizeHint(marginSize);
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
