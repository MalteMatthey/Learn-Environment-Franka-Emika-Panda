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
    const int SIDEBAR_MINIMUM_WIDTH = 120;
    const int SIDEBAR_TITLE_FONT_SIZE = 16;
    const char* SIDEBAR_TITLE_LABEL = "Tasks";

    const QSize SIDEBAR_ITEM_MARGIN_SIZE(0, 30);
    const QSize SIDEBAR_TITLE_ITEM_MARGINS(0, 20);
    const QSize MARGIN_ITEM_SIDEBAR_ITEM_MARGIN_SIZE(0, 10);
    const int SIDEBAR_ITEM_TOPIC_FONT_SIZE = 10;
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
    QFont boldFont;
    boldFont.setBold(true);
    boldFont.setPointSize(SIDEBAR_ITEM_TOPIC_FONT_SIZE);
    QFont normalFont;
    normalFont.setPointSize(SIDEBAR_ITEM_FONT_SIZE);

    listWidget->clear();
    taskIndexToItemMap.clear();

    QString currentTopic;
    bool firstTopic = true;

    for (int i = 0; i < tasks.size(); ++i) {
        QSharedPointer<Task> task = tasks[i];
        if (task->topic != currentTopic) {
            if (!firstTopic) {
                QListWidgetItem *marginItem = new QListWidgetItem("");
                marginItem->setSizeHint(MARGIN_ITEM_SIDEBAR_ITEM_MARGIN_SIZE);
                marginItem->setFlags(Qt::NoItemFlags);
                listWidget->addItem(marginItem);
            }
            firstTopic = false;
            currentTopic = task->topic;
            QListWidgetItem *topicItem = new QListWidgetItem(capitalize(currentTopic));
            topicItem->setFont(boldFont);
            topicItem->setSizeHint(SIDEBAR_TITLE_ITEM_MARGINS);
            topicItem->setFlags(Qt::NoItemFlags);
            topicItem->setForeground(QBrush(Qt::black));
            listWidget->addItem(topicItem);
        }

        QListWidgetItem *taskItem = new QListWidgetItem(task->title);
        taskItem->setFont(normalFont);
        taskItem->setSizeHint(SIDEBAR_ITEM_MARGIN_SIZE);
        taskItem->setData(Qt::UserRole, i);
        listWidget->addItem(taskItem);
        taskIndexToItemMap[i] = taskItem;
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
    if (taskIndexToItemMap.contains(taskIndex)) {
        QListWidgetItem *item = taskIndexToItemMap[taskIndex];
        listWidget->clearSelection();
        item->setSelected(true);
        listWidget->scrollToItem(item);
    } else {
        qWarning() << "Task with index" << taskIndex << "not found in the list.";
    }
}