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

void Learn_Window::onExecuteButtonClicked() {

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

    checkResult();
}

void Learn_Window::checkResult() {
    ros::Duration(2.0).sleep();

    sensor_msgs::JointStateConstPtr msg = ros::topic::waitForMessage<sensor_msgs::JointState>("/joint_states", nh_, ros::Duration(5.0));

    if (msg) {
        QString joint_states;
        for (size_t i = 0; i < std::min(msg->name.size(), size_t(7)); ++i) {
            joint_states += QString::fromStdString(msg->name[i]) + ": " + QString::number(msg->position[i]) + "\n";
        }
        ROS_INFO("Joint States:\n%s", joint_states.toStdString().c_str());

        // Compare to expected values
        bool match = true;

        // Define expected values here
        // Define expected values for each joint
        std::vector<double> expected_positions = {
            -1.38089706712302,  // joint 1
            1.7626998758091972, // joint 2
            1.774931698337621,  // joint 3
            -2.262299837416762, // joint 4
            1.5498787547996722, // joint 5
            1.8493953485899208, // joint 6
            -0.8594281783760449 // joint 7
        };

        double error_margin = 0.01;

        if (msg->position.size() < 7) {
            match = false;

        } else {
            for (size_t i = 0; i < 7; ++i) {
                if (std::abs(msg->position[i] - expected_positions[i]) > error_margin) {
                    ROS_WARN("panda_joint%lu: %f does not match expected value", i + 1, msg->position[i]);
                    match = false;
                    break;
                }
            }
        }

        if (match) {
            ROS_INFO("Joint states match expected values.");
        } else {
            ROS_ERROR("Joint states do not match expected values.");
        }

    } else {
        ROS_ERROR("Failed to receive joint states message.");
    }
}

PLUGINLIB_EXPORT_CLASS(Learn_Window, rviz::Panel)