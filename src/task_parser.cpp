#include "learn_environment/task_parser.h"

#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

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

    for (const auto& taskJson : jsonData["tasks"]) {
        if (!taskJson.is_object()) {
            qCritical() << "Invalid task format";
            continue;
        }

        QSharedPointer<Task> task(new Task);
        try {
            task->title = QString::fromStdString(taskJson.at("title").get<std::string>());
            task->description = QString::fromStdString(taskJson.at("description").get<std::string>());
            task->folder = QString::fromStdString(taskJson.at("folder").get<std::string>());
            task->difficulty = QString::fromStdString(taskJson.at("difficulty").get<std::string>());

            // Optional fields
            if (taskJson.contains("previous_subtasks_required")) {
                task->previousSubtasksRequired = taskJson.at("previous_subtasks_required").get<bool>();
            }
        } catch (const json::type_error& e) {
            qCritical() << "Type error in task:" << e.what();
            continue;
        }

        if (taskJson.contains("subtasks") && taskJson["subtasks"].is_array()) {
            task->subtasks = parseSubtasks(taskJson["subtasks"], task);
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
            subtask.title = QString::fromStdString(subtaskJson.at("title").get<std::string>());
            subtask.description = QString::fromStdString(subtaskJson.at("description").get<std::string>());
            subtask.file = QString::fromStdString(subtaskJson.at("file").get<std::string>());
            subtask.solutionFilePath = QString::fromStdString(subtaskJson.at("solution_file_path").get<std::string>());
            subtask.evaluationFilePath = QString::fromStdString(subtaskJson.at("evaluation_file_path").get<std::string>());

            // Optional fields
            if (subtaskJson.contains("timeout_seconds")) {
                subtask.timeoutSeconds = subtaskJson.at("timeout_seconds").get<int>();
            }
            if (subtaskJson.contains("parallelized_evaluation_required")) {
                subtask.parallelizedEvaluationRequired = subtaskJson.at("parallelized_evaluation_required").get<bool>();
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