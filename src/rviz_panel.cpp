#include "learn_environment/rviz_panel.hpp"
#include <pluginlib/class_list_macros.hpp>
#include <QProcess>
#include <QFile>
#include <QString>
#include <QDebug>
#include <ros/ros.h>
#include <cstdlib>
#include <moveit/move_group_interface/move_group_interface.h>
#include <geometry_msgs/Pose.h>
#include <ros/package.h>
#include <sensor_msgs/JointState.h>
#include <QRegularExpression>



Learn_Window::Learn_Window(QWidget *parent)
    : rviz::Panel(parent), ui(new Ui::Learn_Window), process(new QProcess(this)) {
    ui->setupUi(this);
    connect(ui->executeButton, SIGNAL(clicked()), this, SLOT(onExecuteButtonClicked()));
}

Learn_Window::~Learn_Window() {
    delete ui;
}

void Learn_Window::load(const rviz::Config &config) {
    rviz::Panel::load(config);
}

void Learn_Window::save(rviz::Config config) const {
    rviz::Panel::save(config);
}

void Learn_Window::onExecuteButtonClicked() {
    QString packagePath = QString::fromStdString(ros::package::getPath("learn_environment"));

    QString notebookPath = packagePath + "/tutorial/beginner/exercise_1/exercise_1_1.ipynb";
    QString convertScriptPath = packagePath + "/converter/convert.py";
    QString convertedScriptPath = packagePath + "/converter/converted.py";
    QString evalScriptPath = packagePath + "/tutorial/beginner/exercise_1/exercise_1_1_eval.py";

    // step 1: check if the notebook file exists
    if (!QFile::exists(notebookPath)) {
        ROS_ERROR("Jupyter Notebook file not found: %s", notebookPath.toStdString().c_str());
        return;
    }

    // step 2: check if the convert script file exists
    if (!QFile::exists(convertScriptPath)) {
        ROS_ERROR("Converter script not found: %s", convertScriptPath.toStdString().c_str());
        return;
    }

    // step 3: convert the notebook file
    QStringList convertArgs;
    convertArgs << notebookPath;
    QString convertOutput = executePythonScript(convertScriptPath, convertArgs);
    if (convertOutput.startsWith("Error")) {
        ROS_ERROR(convertOutput.toStdString().c_str());
        return;
    }

    ROS_INFO("Conversion completed successfully.");
    ROS_INFO("Starting converted script: %s", convertedScriptPath.toStdString().c_str());

    QString scriptOutput = executePythonScript(convertedScriptPath, QStringList());

    if (scriptOutput.startsWith("Error")) {
        ROS_ERROR(scriptOutput.toStdString().c_str());
        return;
    }

    ROS_INFO("Converted script executed successfully.");
    
    checkResult(evalScriptPath);
}

QString Learn_Window::executePythonScript(QString path, QStringList args) {
    
    // Check if the file exists
    if(!QFile::exists(path)) {
        return QString("Error, python script not found: %1").arg(path);
    }

    QProcess executeProcess;
    executeProcess.start("python3", QStringList() << path << args);
    if (!executeProcess.waitForFinished()) {
        return QString("Error, python script took to long: %1").arg(path.toStdString().c_str());
    }

    // Check for errors
    QString errorOutput = executeProcess.readAllStandardError();
    if (!errorOutput.isEmpty()) {
        return QString("Error: %1").arg(errorOutput);
    }

    // Return the output
    QString output = executeProcess.readAllStandardOutput().trimmed();
    return output;
}

void Learn_Window::checkResult(QString evalScriptPath) {
    ros::Duration(1.0).sleep();

    QString evalOutput = executePythonScript(evalScriptPath, QStringList());

    if (evalOutput.isEmpty()) {
        ROS_ERROR("Evaluation script did not return any output.");

    } else if (evalOutput.startsWith("true")) {
        ROS_INFO("The evaluation is positiv.");

    } else if (evalOutput.startsWith("false")) {
        QString errorMsg = evalOutput.split(',').at(1);
        ROS_WARN("The evaluation is negativ: %s", errorMsg.toStdString().c_str());

    } else if (evalOutput.startsWith("Error")) {
        ROS_ERROR(evalOutput.toStdString().c_str());

    } else {
        ROS_WARN("Unknown output from evaluation script: %s", evalOutput.toStdString().c_str());
    }
}

PLUGINLIB_EXPORT_CLASS(Learn_Window, rviz::Panel)