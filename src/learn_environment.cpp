#include "learn_environment/learn_environment.h"
#include <pluginlib/class_list_macros.hpp>
#include "learn_environment/task_manager.h"
#include "learn_environment/task_ui.h"

#include <QMainWindow>
#include <QSplitter>
#include <QHBoxLayout>

LearnEnvironment::LearnEnvironment(QWidget *parent)
    : rviz::Panel(parent),
      ui(new Ui::LearnEnvironment),
      process(new QProcess(this)),
      taskManager(nullptr),
      taskUI(nullptr)
{
    ui->setupUi(this);
    initializeUI();
}

LearnEnvironment::~LearnEnvironment()
{
    delete ui;
    delete process;
}

void LearnEnvironment::initializeUI() {
    sidebar = new Sidebar(this);
    taskUI = new TaskUI(
        ui->subtaskListWidget,
        ui->mainTitleLabel,
        ui->difficultyLabel,
        ui->folderLabel,
        ui->nextButton,
        ui->previousButton,
        *sidebar,
        this
    );
    taskManager = new TaskManager(taskUI, ui->nextButton, ui->previousButton, this);

    setupSplitterAndLayout();

    sidebar->setVisible(false);

    connect(ui->menuButton, &QPushButton::clicked, this, &LearnEnvironment::toggleSidebarVisibility);
}

void LearnEnvironment::setupSplitterAndLayout() {
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

void LearnEnvironment::toggleSidebarVisibility() {
    bool isVisible = sidebar->isVisible();
    sidebar->setVisible(!isVisible);

    if (sidebar->isVisible()) {
        ui->menuButton->setIcon(QIcon(":/resource/icons/close.png"));
    } else {
        ui->menuButton->setIcon(QIcon(":/resource/icons/menu.png"));
    }
}

void LearnEnvironment::load(const rviz::Config &config) {
    // Implement loading of user-specific settings
}

void LearnEnvironment::save(rviz::Config config) const {
    // Implement saving of user-specific settings
}

PLUGINLIB_EXPORT_CLASS(LearnEnvironment, rviz::Panel)