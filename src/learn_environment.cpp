#include "learn_environment.hpp"
#include <pluginlib/class_list_macros.hpp>

#include <QMainWindow>

LearnEnvironment::LearnEnvironment(QWidget *parent)
    : rviz::Panel(parent),
      ui(new Ui::LearnEnvironment),
      process(new QProcess(this)),
      taskManager(nullptr),
      taskUI(nullptr),
      notebookConverter(new NotebookConverter()),
      notebookThread(new QThread())
{
    ui->setupUi(this);
    initialize();
}

LearnEnvironment::~LearnEnvironment()
{
    notebookThread->quit();
    notebookThread->wait();
    delete notebookConverter;
    delete notebookThread;
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

    notebookConverter->moveToThread(notebookThread);
    connect(notebookThread, &QThread::started, notebookConverter, &NotebookConverter::processTaskPool);

    notebookThread->start();
}

void LearnEnvironment::load(const rviz::Config &config) {
    // Implement loading of user-specific settings
}

void LearnEnvironment::save(rviz::Config config) const {
    // Implement saving of user-specific settings
}

PLUGINLIB_EXPORT_CLASS(LearnEnvironment, rviz::Panel)