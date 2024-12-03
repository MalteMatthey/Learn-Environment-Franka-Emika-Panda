// learning_plattform.cpp
#include "learn_environment/learning_plattform.h"
#include <pluginlib/class_list_macros.hpp>
#include "learn_environment/task_manager.h"
#include <QMainWindow>
#include <QSplitter>

LearningPlattform::LearningPlattform(QWidget *parent)
    : rviz::Panel(parent)
    , ui(new Ui::LearningPlattform)
    , process(new QProcess(this))
    , taskManager(nullptr)
{
    ui->setupUi(this);
    initializeUI();
}

LearningPlattform::~LearningPlattform()
{
    delete ui;
    delete taskManager;
    delete process;
}

void LearningPlattform::initializeUI() {
    sidebar = new Sidebar(this);
    taskManager = new TaskManager(
        *sidebar,
        ui->subtaskListWidget,
        ui->mainTitleLabel,
        ui->difficultyLabel,
        ui->folderLabel,
        ui->nextButton,
        ui->previousButton
    );

    setupSplitterAndLayout();

    sidebar->setVisible(false);

    connect(ui->menuButton, &QPushButton::clicked, this, &LearningPlattform::toggleSidebarVisibility);
}

void LearningPlattform::setupSplitterAndLayout() {
    QSplitter *splitter = new QSplitter(Qt::Horizontal, this);

    splitter->addWidget(sidebar);
    splitter->addWidget(ui->centralwidget);

    QList<int> sizes;
    sizes << 200 << 600;
    splitter->setSizes(sizes);

    // Set the layout to the main widget
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(splitter);
    setLayout(mainLayout);
}

void LearningPlattform::toggleSidebarVisibility() {
    bool isVisible = sidebar->isVisible();
    sidebar->setVisible(!isVisible);

    if (sidebar->isVisible()) {
        ui->menuButton->setIcon(QIcon(":/resource/icons/close.png"));
    } else {
        ui->menuButton->setIcon(QIcon(":/resource/icons/menu.png"));
    }
}

void LearningPlattform::load(const rviz::Config &config) {
    // Implement loading of user-specific settings
}

void LearningPlattform::save(rviz::Config config) const {
    // Implement saving of user-specific settings
}

PLUGINLIB_EXPORT_CLASS(LearningPlattform, rviz::Panel)