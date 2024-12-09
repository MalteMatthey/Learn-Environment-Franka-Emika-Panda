#include "learn_environment/task_parser.h"

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
    const char* PREVIOUS_SUBTASKS_REQUIRED_KEY = "previous_subtasks_required";
    const char* SUBTASKS_KEY = "subtasks";
    const char* FILE_KEY = "file";
    const char* SOLUTION_FILE_PATH_KEY = "solution_file_path";
    const char* EVALUATION_FILE_PATH_KEY = "evaluation_file_path";
    const char* TIMEOUT_SECONDS_KEY = "timeout_seconds";
    const char* PARALLELIZED_EVALUATION_REQUIRED_KEY = "parallelized_evaluation_required";
}

QVector<QSharedPointer<Task>> TaskParser::loadTasks(const QString& filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qCritical() << "Could not open file:" << filePath;
        return {};
    }

    QTextStream in(&file);
    QString jsonString = in.readAll();
    file.close();

    json jsonData;
    try {
        jsonData = json::parse(jsonString.toStdString());
    } catch (const json::parse_error& e) {
        qCritical() << "JSON parse error:" << e.what();
        return {};
    }

    return parseTasks(jsonData);
}

QVector<QSharedPointer<Task>> TaskParser::parseTasks(const json& jsonData) {
    QVector<QSharedPointer<Task>> tasks;

    for (const auto& taskJson : jsonData[TASKS_KEY]) {
        if (!taskJson.is_object()) {
            qCritical() << "Invalid task format";
            continue;
        }

        QSharedPointer<Task> task(new Task);
        try {
            task->title = QString::fromStdString(taskJson.at(TITLE_KEY).get<std::string>());
            task->description = QString::fromStdString(taskJson.at(DESCRIPTION_KEY).get<std::string>());
            task->folder = QString::fromStdString(taskJson.at(FOLDER_KEY).get<std::string>());
            task->difficulty = QString::fromStdString(taskJson.at(DIFFICULTY_KEY).get<std::string>());

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
            subtask.title = QString::fromStdString(subtaskJson.at(TITLE_KEY).get<std::string>());
            subtask.description = QString::fromStdString(subtaskJson.at(DESCRIPTION_KEY).get<std::string>());
            subtask.file = QString::fromStdString(subtaskJson.at(FILE_KEY).get<std::string>());
            subtask.solutionFilePath = QString::fromStdString(subtaskJson.at(SOLUTION_FILE_PATH_KEY).get<std::string>());
            subtask.evaluationFilePath = QString::fromStdString(subtaskJson.at(EVALUATION_FILE_PATH_KEY).get<std::string>());

            // Optional fields
            if (subtaskJson.contains(TIMEOUT_SECONDS_KEY)) {
                subtask.timeoutSeconds = subtaskJson.at(TIMEOUT_SECONDS_KEY).get<int>();
            }
            if (subtaskJson.contains(PARALLELIZED_EVALUATION_REQUIRED_KEY)) {
                subtask.parallelizedEvaluationRequired = subtaskJson.at(PARALLELIZED_EVALUATION_REQUIRED_KEY).get<bool>();
            }

            // Set the parent task for the subtask
            subtask.parentTask = parentTask;
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