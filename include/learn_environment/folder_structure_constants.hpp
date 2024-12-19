#ifndef FOLDER_STRUCTURE_CONSTANTS_HPP
#define FOLDER_STRUCTURE_CONSTANTS_HPP

#include <QString>
#include <ros/package.h>
#include <ros/ros.h>
#include <QDebug>

/**
 * @namespace FolderStructureConstants
 * @brief Contains constants related to the folder structure of the learn_environment package.
 *
 * This namespace provides constants for various paths used within the learn_environment package,
 * as well as a function to retrieve the package path.
 */
namespace FolderStructureConstants {
    /**
     * @var FolderStructureConstants::CONVERTED_SCRIPT_PATH
     * @brief Path to the converted script.
     */
    inline const QString CONVERTED_SCRIPT_PATH = "/converter/converted.py";

    /**
     * @var FolderStructureConstants::RESET_ROBOT_SCRIPT_PATH
     * @brief Path to the reset robot script.
     */
    inline const QString RESET_ROBOT_SCRIPT_PATH = "/task_pool/reset_robot.py";

    /**
     * @var FolderStructureConstants::TASK_DEFINITIONS_PATH
     * @brief Path to the task definitions json.
     */
    inline const QString TASK_DEFINITIONS_PATH = ":/task_pool/task_definitions.json";

    /**
     * @var FolderStructureConstants::TOPIC_DEFINITIONS_PATH
     * @brief Path to the topic definitions json.
     */
    inline const QString TOPIC_DEFINITIONS_PATH = ":/task_pool/topic_definitions.json";

    /**
     * @var FolderStructureConstants::TOPIC_DEFINITIONS_PATH
     * @brief Path to the difficulty definitions json.
     */
    inline const QString DIFFICULTY_LEVELS_DEFINITION_PATH = ":/task_pool/difficulty_levels.json";

    /**
     * @var FolderStructureConstants::SOLUTION_SCRIPTS_SOURCE_PATH
     * @brief Path to the solution scripts source directory.
     */
    inline const QString SOLUTION_SCRIPTS_SOURCE_PATH = "/task_pool/solution_scripts";

    /**
     * @var FolderStructureConstants::EVALUATION_SCRIPTS_SOURCE_PATH
     * @brief Path to the evaluation scripts source directory.
     */
    inline const QString EVALUATION_SCRIPTS_SOURCE_PATH = "/task_pool/evaluation_scripts";

    /**
     * @var FolderStructureConstants::USER_WORKSPACE
     * @brief Path to the user workspace directory. Adjust it in devconatiner.json if changed.
     */
    inline const QString USER_WORKSPACE = "/tasks";

    /**
     * @var FolderStructureConstants::PACKAGE_NAME
     * @brief Name of the package.
     */
    inline const QString PACKAGE_NAME = "learn_environment";

    /**
     * @fn QString FolderStructureConstants::getPackagePath()
     * @brief Retrieves the path of the package.
     *
     * This function attempts to retrieve the path of the package using ROS package utilities.
     * If an error occurs during retrieval, it logs a critical error message and returns an empty QString.
     *
     * @return QString The path of the package or an empty QString if an error occurs.
     */
    inline QString getPackagePath() {
        try {
            return QString::fromStdString(ros::package::getPath(PACKAGE_NAME.toStdString()));
        } catch (...) {
            qCritical() << "An error occurred while retrieving the package path for" << PACKAGE_NAME;
            return QString();
        }
    }
}

#endif // FOLDER_STRUCTURE_CONSTANTS_HPP