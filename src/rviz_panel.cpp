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
    QString notebookPath = packagePath + "/tutorial/intermediate/pick_place.ipynb";
    QString convertScriptPath = packagePath + "/converter/convert.py";
    QString convertedScriptPath = packagePath + "/converter/converted.py";

    // Überprüfen ob Skript gibt
    if (!QFile::exists(notebookPath)) {
        ROS_INFO("File not found: %s", notebookPath.toStdString().c_str());
        return;
    }


    // Schritt 1: Überprüfen, ob die Convert-Skript-Datei existiert
    if (!QFile::exists(convertScriptPath)) {
        ROS_INFO("File not found: %s", convertScriptPath.toStdString().c_str());
        return;
    }

    ROS_INFO("Starting conversion script: %s", convertScriptPath.toStdString().c_str());

    // Schritt 2: Ausführen der `convert`-Funktion auf dem übergebenen Jupyter-Notebook
    std::string convertCommand = "python3 " + convertScriptPath.toStdString() + " " + notebookPath.toStdString();
    int convertResult = std::system(convertCommand.c_str());
    if (convertResult != 0) {
        ROS_ERROR("Failed to execute conversion script: %s", convertScriptPath.toStdString().c_str());
        ROS_ERROR("Conversion script raises an exception!!!");
        return;
    }

    ROS_INFO("Conversion completed successfully.");

    // Schritt 3: Überprüfen, ob die Converted-Skript-Datei existiert
    if (!QFile::exists(convertedScriptPath)) {
        ROS_INFO("File not found: %s", convertedScriptPath.toStdString().c_str());
        return;
    }

    ROS_INFO("Starting converted script: %s", convertedScriptPath.toStdString().c_str());

    // Schritt 4: Ausführen der Datei `converted.py`
    std::string convertedCommand = "python3 " + convertedScriptPath.toStdString();
    int convertedResult = std::system(convertedCommand.c_str());
    if (convertedResult != 0) {
        ROS_ERROR("Failed to execute converted script: %s", convertedScriptPath.toStdString().c_str());
        ROS_ERROR("Converted script raises an exception!!!");
        return;
    }

    ROS_INFO("Converted script executed successfully.");
    
    checkResult();
}

void Learn_Window::checkResult() {
    ros::Duration(1.0).sleep();

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
            0.0,    // joint 1
            -0.785, // joint 2
            0.0,    // joint 3
            -2.355, // joint 4
            2.705,    // joint 5
            1.57,   // joint 6
            0.785   // joint 7
        };

        const double error_margin = 0.01;

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
            ROS_ERROR("Joint states do not match expected values. Maybe you should study Maschinenbau.");
        }

    } else {
        ROS_ERROR("Failed to receive joint states message.");
    }
}

PLUGINLIB_EXPORT_CLASS(Learn_Window, rviz::Panel)