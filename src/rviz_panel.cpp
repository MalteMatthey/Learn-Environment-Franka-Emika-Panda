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


Learn_Window::Learn_Window(QWidget *parent)
    : rviz::Panel(parent), ui(new Ui::Learn_Window), process(new QProcess(this)) {
        ui ->setupUi(this);
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

void Learn_Window::onExecuteButtonClicked() {}

    QString packagePath = QString::fromStdString(ros::package::getPath("learn_environment"));
    ROS_INFO("Package path: %s", packagePath.toStdString().c_str());
    QString scriptPath = packagePath + "/tasks/exercise_1.py";

    if (!QFile::exists(scriptPath)) {
        ROS_INFO("File not found: %s", scriptPath.toStdString().c_str());
        return;
    }

    ROS_INFO("Starting script: %s", scriptPath.toStdString().c_str());

    std::string command = "python3 " + scriptPath.toStdString();
    int result = std::system(command.c_str());
    if (result != 0) {
        ROS_ERROR("Failed to execute script: %s", scriptPath.toStdString().c_str());
    }

    ros::Duration(5.0).sleep();


    checkResult();
}

void Learn_Window::checkResult() {
    ROS_INFO("Checking result");

    // Create a MoveGroupInterface object for the Panda arm
    moveit::planning_interface::MoveGroupInterface move_group("panda_arm");
    move_group.setPlanningTime(10.0);

    geometry_msgs::Pose target_pose;
    target_pose.position.x = 0.5;
    target_pose.position.y = 0.0;
    target_pose.position.z = 0.5;
    target_pose.orientation.x = 0.0;
    target_pose.orientation.y = 0.0;
    target_pose.orientation.z = 0.0;
    target_pose.orientation.w = 1.0;
    
    geometry_msgs::Pose current_pose = move_group.getCurrentPose().pose;

    ROS_INFO("got current position");

    // Log the current position and orientation
    ROS_INFO("Current position: x=%f, y=%f, z=%f", current_pose.position.x, current_pose.position.y, current_pose.position.z);
    ROS_INFO("Current orientation: x=%f, y=%f, z=%f, w=%f", current_pose.orientation.x, current_pose.orientation.y, current_pose.orientation.z, current_pose.orientation.w);

    double tolerance = 0.01;

    bool position_match = std::abs(current_pose.position.x - target_pose.position.x) < tolerance &&
                          std::abs(current_pose.position.y - target_pose.position.y) < tolerance &&
                          std::abs(current_pose.position.z - target_pose.position.z) < tolerance;

    bool orientation_match = std::abs(current_pose.orientation.x - target_pose.orientation.x) < tolerance &&
                             std::abs(current_pose.orientation.y - target_pose.orientation.y) < tolerance &&
                             std::abs(current_pose.orientation.z - target_pose.orientation.z) < tolerance &&
                             std::abs(current_pose.orientation.w - target_pose.orientation.w) < tolerance;

    if (position_match && orientation_match) {
        ROS_INFO("The robot is in the correct position.");
    } else {
        ROS_INFO("The robot is NOT in the correct position.");
    }
}

PLUGINLIB_EXPORT_CLASS(Learn_Window, rviz::Panel)