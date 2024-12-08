#ifndef TASK_PARSER_H
#define TASK_PARSER_H

#include "task.h"

#include <QVector>
#include <QString>
#include <QSharedPointer>
#include <nlohmann/json.hpp>

/**
 * @class TaskParser
 * @brief Parses tasks and subtasks from a JSON file.
 *
 * The TaskParser class is responsible for loading tasks and their subtasks from a JSON file.
 * It provides methods to parse the JSON data and convert it into Task and Subtask objects.
 */
class TaskParser {
public:
    /**
     * @brief Loads tasks from a JSON file.
     * @param filePath The path to the JSON file containing the tasks.
     * @return A vector of shared pointers to the loaded tasks.
     */
    QVector<QSharedPointer<Task>> loadTasks(const QString& filePath);

private:
    using json = nlohmann::json;

    /**
     * @brief Parses tasks from JSON data.
     * @param jsonData The JSON data containing the tasks.
     * @return A vector of shared pointers to the parsed tasks.
     */
    QVector<QSharedPointer<Task>> parseTasks(const json& jsonData);

    /**
     * @brief Parses subtasks from JSON data.
     * @param subtasksJson The JSON data containing the subtasks.
     * @param parentTask A shared pointer to the parent task.
     * @return A vector of parsed subtasks.
     */
    QVector<Subtask> parseSubtasks(const json& subtasksJson, QSharedPointer<Task> parentTask);
};

#endif // TASK_PARSER_H