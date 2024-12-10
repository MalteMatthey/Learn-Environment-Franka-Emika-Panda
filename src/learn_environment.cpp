#include "learn_environment/learn_environment.hpp"
#include "learn_environment/task_manager.hpp"
#include "learn_environment/task_ui.hpp"
#include <pluginlib/class_list_macros.hpp>

#include <QMainWindow>
#include <QSplitter>
#include <QHBoxLayout>

namespace {
    const char* CLOSE_ICON_PATH = ":/resource/icons/close.png";
    const char* MENU_ICON_PATH = ":/resource/icons/menu.png";
}

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

    notebookConverter->moveToThread(notebookThread);
    connect(notebookThread, &QThread::started, notebookConverter, &NotebookConverter::processTaskPool);

    // Optional: Handle thread finished
    // connect(notebookConverter, &NotebookConverter::finished, notebookThread, &QThread::quit);
    // connect(notebookThread, &QThread::finished, notebookConverter, &NotebookConverter::deleteLater);
    // connect(notebookThread, &QThread::finished, notebookThread, &QThread::deleteLater);

    notebookThread->start();
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
        ui->menuButton->setIcon(QIcon(CLOSE_ICON_PATH));
    } else {
        ui->menuButton->setIcon(QIcon(MENU_ICON_PATH));
    }
}

void LearnEnvironment::load(const rviz::Config &config) {
    // Implement loading of user-specific settings
}

void LearnEnvironment::save(rviz::Config config) const {
    // Implement saving of user-specific settings
}

PLUGINLIB_EXPORT_CLASS(LearnEnvironment, rviz::Panel)