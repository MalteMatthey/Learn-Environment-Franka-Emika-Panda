#include "learn_environment/task_parser.hpp"
#include "learn_environment/folder_structure_constants.hpp"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    const char* TASKS_KEY = "tasks";
    const char* TITLE_KEY = "title";
    const char* DESCRIPTION_KEY = "description";
    const char* FOLDER_KEY = "folder";
    const char* DIFFICULTY_KEY = "difficulty";
    const char* TOPIC_KEY = "topic";
    const char* PREVIOUS_SUBTASKS_REQUIRED_KEY = "previous_subtasks_required";
    const char* SUBTASKS_KEY = "subtasks";
    const char* SOLUTION_FILE_KEY = "solution_file";
    const char* EVALUATION_FILE_KEY = "evaluation_file";
    const char* TIMEOUT_SECONDS_KEY = "timeout_seconds";
    const char* PARALLELIZED_EVALUATION_REQUIRED_KEY = "parallelized_evaluation_required";
    const char* RESET_ROBOT_BEFORE_EXECUTING_KEY = "reset_robot_before_executing";
    const char* DIFFICULTY_LEVELS_KEY = "difficulty_levels";
    const char* DIFFICULTY_LEVELS_NAME_KEY = "name";
    const char* DIFFICULTY_LEVELS_HEX_COLOR_KEY = "hex-color";
    const char* TOPICS_DEFINITION_KEY = "topics";
    const char* DEFAULT_DIFFICULTY_COLOR = "#1d0e53";
    const char* DEFAULT_TOPIC_NAME = "unknown";
}

QVector<QSharedPointer<Task>> TaskParser::loadTasks(const QString& taskPath, const QString& difficultyPath, const QString& topicPath) {
    QFile taskFile(taskPath);
    if (!taskFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Could not open task file:" << taskPath;
        return {};
    }

    QTextStream taskIn(&taskFile);
    QString taskJsonString = taskIn.readAll();
    taskFile.close();

    QFile difficultyFile(difficultyPath);
    if (!difficultyFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Could not open difficulty file:" << difficultyPath;
        return {};
    }

    QTextStream difficultyIn(&difficultyFile);
    QString difficultyJsonString = difficultyIn.readAll();
    difficultyFile.close();

    QFile topicFile(topicPath);
    if (!topicFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Could not open topic file:" << topicPath;
        return {};
    }

    QTextStream topicIn(&topicFile);
    QString topicJsonString = topicIn.readAll();
    topicFile.close();

    json taskJsonData;
    json difficultyJsonData;
    json topicJsonData;
    try {
        taskJsonData = json::parse(taskJsonString.toStdString());
        difficultyJsonData = json::parse(difficultyJsonString.toStdString());
        topicJsonData = json::parse(topicJsonString.toStdString());
    } catch (const json::parse_error& e) {
        qCritical() << "JSON parse error:" << e.what();
        return {};
    }

    return parseTasks(taskJsonData, difficultyJsonData, topicJsonData);
}

QVector<QSharedPointer<Task>> TaskParser::parseTasks(const json& taskJsonData, const json& difficultyJsonData, const json& topicJsonData) {
    QVector<QSharedPointer<Task>> tasks;

    for (const auto& taskJson : taskJsonData[TASKS_KEY]) {
        if (!taskJson.is_object()) {
            qCritical() << "Invalid task format";
            continue;
        }

        QSharedPointer<Task> task(new Task);
        try {
            task->title = QString::fromStdString(taskJson.at(TITLE_KEY).get<std::string>());
            task->difficulty = QString::fromStdString(taskJson.at(DIFFICULTY_KEY).get<std::string>());
            task->folder = QString::fromStdString(taskJson.at(FOLDER_KEY).get<std::string>());

            // Set topic if defined in topic_definitions.json, else set to unknown
            QString topic = QString::fromStdString(taskJson.at(TOPIC_KEY).get<std::string>());
            bool topicFound = false;

            if (topicJsonData.contains(TOPICS_DEFINITION_KEY) && topicJsonData[TOPICS_DEFINITION_KEY].is_array()) {
                for (const auto& t : topicJsonData[TOPICS_DEFINITION_KEY]) {
                    if (t.is_string() && t.get<std::string>() == topic.toStdString()) {
                        topicFound = true;
                        break;
                    }
                }
            }

            if (topicFound) {
                task->topic = topic;
            } else {
                task->topic = QString::fromStdString(DEFAULT_TOPIC_NAME);
            }

            // Set difficultyHexColor based on difficulty
            task->difficultyHexColor = DEFAULT_DIFFICULTY_COLOR;
            for (const auto& level : difficultyJsonData[DIFFICULTY_LEVELS_KEY]) {
                if (level[DIFFICULTY_LEVELS_NAME_KEY].get<std::string>() == task->difficulty.toStdString()) {
                    task->difficultyHexColor = QString::fromStdString(level[DIFFICULTY_LEVELS_HEX_COLOR_KEY].get<std::string>());
                    break;
                }
            }

            // Optional fields
            if (taskJson.contains(PREVIOUS_SUBTASKS_REQUIRED_KEY)) {
                task->previousSubtasksRequired = taskJson.at(PREVIOUS_SUBTASKS_REQUIRED_KEY).get<bool>();
            }
        } catch (const json::type_error& e) {
            qCritical() << "Type error in task:" << e.what();
            continue;
        }

        if (taskJson.contains(SUBTASKS_KEY) && taskJson[SUBTASKS_KEY].is_array()) {
            task->subtasks = parseSubtasks(taskJson[SUBTASKS_KEY], task);
        } else {
            qCritical() << "Task does not contain valid 'subtasks'";
        }

        tasks.push_back(task);
    }

    // Create a map of topic to its order based on topic_definitions.json
    QHash<QString, int> topicOrderMap;
    int order = 0;

    if (topicJsonData.contains(TOPICS_DEFINITION_KEY) && topicJsonData[TOPICS_DEFINITION_KEY].is_array()) {
        for (const auto& topic : topicJsonData[TOPICS_DEFINITION_KEY]) {
            if (topic.is_string()) {
                QString topicName = QString::fromStdString(topic.get<std::string>());
                topicOrderMap[topicName] = order++;
            } else {
                qCritical() << "Invalid topic definition. Expected a string.";
            }
        }
    }

    // Sort tasks based on the defined topic order
    std::sort(tasks.begin(), tasks.end(), [&](const QSharedPointer<Task>& a, const QSharedPointer<Task>& b) -> bool {
        int orderA = topicOrderMap.value(a->topic, order);
        int orderB = topicOrderMap.value(b->topic, order);
        return orderA < orderB;
    });

    return tasks;
}

QVector<Subtask> TaskParser::parseSubtasks(const json& subtasksJson, QSharedPointer<Task> parentTask) {
    QVector<Subtask> subtasks;

    for (const auto& subtaskJson : subtasksJson) {
        if (!subtaskJson.is_object()) {
            qCritical() << "Invalid subtask format";
            continue;
        }

        Subtask subtask;
        try {
            // Set the parent task for the subtask
            subtask.parentTask = parentTask;

            subtask.title = QString::fromStdString(subtaskJson.at(TITLE_KEY).get<std::string>());
            subtask.description = QString::fromStdString(subtaskJson.at(DESCRIPTION_KEY).get<std::string>());
            subtask.file = QString::fromStdString(subtaskJson.at(SOLUTION_FILE_KEY).get<std::string>());
            subtask.filePath = FolderStructureConstants::getPackagePath() 
                               + FolderStructureConstants::USER_WORKSPACE + parentTask->folder 
                               + QString::fromStdString(subtaskJson.at(SOLUTION_FILE_KEY).get<std::string>());
            subtask.solutionFilePath = FolderStructureConstants::getPackagePath() 
                                       + FolderStructureConstants::SOLUTION_SCRIPTS_SOURCE_PATH + parentTask->folder
                                       + QString::fromStdString(subtaskJson.at(SOLUTION_FILE_KEY).get<std::string>());
            subtask.evaluationFilePath = FolderStructureConstants::getPackagePath() 
                                         + FolderStructureConstants::EVALUATION_SCRIPTS_SOURCE_PATH + parentTask->folder 
                                         + QString::fromStdString(subtaskJson.at(EVALUATION_FILE_KEY).get<std::string>());
            // Optional fields
            if (subtaskJson.contains(TIMEOUT_SECONDS_KEY)) {
                subtask.timeoutSeconds = subtaskJson.at(TIMEOUT_SECONDS_KEY).get<int>();
            }
            if (subtaskJson.contains(PARALLELIZED_EVALUATION_REQUIRED_KEY)) {
                subtask.parallelizedEvaluationRequired = subtaskJson.at(PARALLELIZED_EVALUATION_REQUIRED_KEY).get<bool>();
            }
            if (subtaskJson.contains(RESET_ROBOT_BEFORE_EXECUTING_KEY)) {
                subtask.reset_robot_before_executing = subtaskJson.at(RESET_ROBOT_BEFORE_EXECUTING_KEY).get<bool>();
            }
        } catch (const json::type_error& e) {
            qCritical() << "Type error in subtask:" << e.what();
            continue;
        } catch (const json::out_of_range& e) {
            qCritical() << "Missing key in subtask:" << e.what();
            continue;
        }

        subtasks.push_back(subtask);
    }

    return subtasks;
}