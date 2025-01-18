#include "learn_environment.hpp"
#include <pluginlib/class_list_macros.hpp>

#include <QMainWindow>

LearnEnvironment::LearnEnvironment(QWidget *parent)
    : rviz::Panel(parent),
      ui(new Ui::LearnEnvironment),
      process(new QProcess(this)),
      taskManager(nullptr),
      taskUI(nullptr)
{
    ui->setupUi(this);
    initialize();
}

LearnEnvironment::~LearnEnvironment()
{
    delete ui;
    delete process;
}

void LearnEnvironment::initialize() {
    taskUI = new TaskUI(
        ui->subtaskListVLayout,
        ui->mainTitleLabel,
        ui->difficultyLabel,
        ui->folderLabel,
        ui->topicLabel,
        ui->nextButton,
        ui->previousButton,
        ui->menuButton,
        ui->resetRobotStartButton,
        ui->resetRobotFrame,
        ui->centralwidget,
        this
    );
    taskManager = new TaskManager(taskUI, this);

    QHBoxLayout *mainPanelLayout = new QHBoxLayout(this);
    mainPanelLayout->addWidget(taskUI);
    mainPanelLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(mainPanelLayout);
}

void LearnEnvironment::load(const rviz::Config &config) {
    // Implement loading of user-specific settings
}

void LearnEnvironment::save(rviz::Config config) const {
    // Implement saving of user-specific settings
}

PLUGINLIB_EXPORT_CLASS(LearnEnvironment, rviz::Panel)