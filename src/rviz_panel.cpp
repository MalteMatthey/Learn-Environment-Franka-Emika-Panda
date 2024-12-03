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

    // step 1: check if the notebook file exists
    if (!QFile::exists(notebookPath)) {
        ROS_INFO("File not found: %s", notebookPath.toStdString().c_str());
        return;
    }

    // step 2: check if the convert script file exists
    if (!QFile::exists(convertScriptPath)) {
        ROS_INFO("File not found: %s", convertScriptPath.toStdString().c_str());
        return;
    }

    // step 3: convert the notebook file
    std::string convertCommand = "python3 " + convertScriptPath.toStdString() + " " + notebookPath.toStdString();
    int convertResult = std::system(convertCommand.c_str());
    if (convertResult != 0) {
        ROS_ERROR("Failed to convert notebook: %s", convertScriptPath.toStdString().c_str());
        return;
    }

    ROS_INFO("Conversion completed successfully.");
    ROS_INFO("Starting converted script: %s", convertedScriptPath.toStdString().c_str());

    // step 4: execute the converted script
    process->start("python3", QStringList() << convertedScriptPath);
    if (!process->waitForFinished()) {
        ROS_ERROR("Failed to execute converted script: %s", convertedScriptPath.toStdString().c_str());
        return;
    }

    QString errorOutput = process->readAllStandardError();
    if (!errorOutput.isEmpty()) {
        ROS_INFO("Error in the script: %s", errorOutput.toStdString().c_str());
        return;
    }

    ROS_INFO("Converted script executed successfully.");
    
    checkResult();
}

void Learn_Window::checkResult() {
    ros::Duration(1.0).sleep();

    QProcess evalProcess;
    QString packagePath = QString::fromStdString(ros::package::getPath("learn_environment"));
    QString evalScriptPath = packagePath + "/tutorial/beginner/exercise_1/exercise_1_1_eval.py";

    if (!QFile::exists(evalScriptPath)) {
        ROS_ERROR("Evaluation script not found: %s", evalScriptPath.toStdString().c_str());
        return;
    }

    evalProcess.start("python3", QStringList() << evalScriptPath);
    if (!evalProcess.waitForFinished()) {
        ROS_ERROR("Failed to execute evaluation script: %s", evalScriptPath.toStdString().c_str());
        return;
    }

    QString rawOutput = evalProcess.readAllStandardOutput().trimmed();

    if (rawOutput.isEmpty()) {
        ROS_ERROR("Evaluation script did not return any output.");
        return;
    }

    ROS_INFO("Evaluation script output: %s", rawOutput.toStdString().c_str());

    if (rawOutput.startsWith("true")) {
        ROS_INFO("Joint state matches expected values.");
    } else {
        QString errorMsg = rawOutput.split(',').at(1);
        ROS_ERROR("Joint state does not match expected values: %s", errorMsg.toStdString().c_str());
    }
}


void Learn_Window::checkResult2() {
    ros::Duration(1.0).sleep();

    // Create an instance of MoveGroupCommander for controlling the Panda arm
    moveit::planning_interface::MoveGroupInterface move_group("panda_arm");

    // Get the current pose of the end effector
    geometry_msgs::Pose current_pose = move_group.getCurrentPose().pose;

    ROS_INFO("Current Pose:");
    ROS_INFO("  Position -> x: %.3f, y: %.3f, z: %.3f", 
             current_pose.position.x, 
             current_pose.position.y, 
             current_pose.position.z);
    ROS_INFO("  Orientation -> x: %.3f, y: %.3f, z: %.3f, w: %.3f", 
             current_pose.orientation.x, 
             current_pose.orientation.y, 
             current_pose.orientation.z, 
             current_pose.orientation.w);

    // Define the expected pose
    geometry_msgs::Pose expected_pose;
    expected_pose.position.x = 0.5;
    expected_pose.position.y = 0.0;
    expected_pose.position.z = 0.2;
    expected_pose.orientation.x = 0.5;
    expected_pose.orientation.y = 0.5;
    expected_pose.orientation.z = 0.5;
    expected_pose.orientation.w = 0.5;

    // Define the error margin
    const double position_error_margin = 0.01;
    const double orientation_error_margin = 0.01;

    // Compare the current pose to the expected pose
    bool match = true;

    if (std::abs(current_pose.position.x - expected_pose.position.x) > position_error_margin ||
        std::abs(current_pose.position.y - expected_pose.position.y) > position_error_margin ||
        std::abs(current_pose.position.z - expected_pose.position.z) > position_error_margin) {
        ROS_WARN("Position does not match expected values.");
        match = false;
    }

    if (std::abs(current_pose.orientation.x - expected_pose.orientation.x) > orientation_error_margin ||
        std::abs(current_pose.orientation.y - expected_pose.orientation.y) > orientation_error_margin ||
        std::abs(current_pose.orientation.z - expected_pose.orientation.z) > orientation_error_margin ||
        std::abs(current_pose.orientation.w - expected_pose.orientation.w) > orientation_error_margin) {
        ROS_WARN("Orientation does not match expected values.");
        match = false;
    }

    if (match) {
        ROS_INFO("Cartesian and orientation pose match expected values.");
    } else {
        ROS_ERROR("Cartesian and orientation pose do not match expected values.");
    }
}

PLUGINLIB_EXPORT_CLASS(Learn_Window, rviz::Panel)