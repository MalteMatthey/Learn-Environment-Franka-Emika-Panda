#ifndef TASK_PARSER_HPP
#define TASK_PARSER_HPP

#include "common/task.hpp"

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
     * @brief Loads tasks with all configuration JSON files.
     * 
     * This function reads the tasks, difficulty levels, and topic definitions from the specified JSON files.
     * It parses the JSON data and constructs a vector of shared pointers to Task objects.
     * The tasks are sorted based on the defined topic order from the topic definition JSON file.
     * If any of the files cannot be opened or if there is a JSON parsing error, an empty vector is returned.
     * 
     * @param taskPath The path to the JSON file containing the tasks.
     * @param difficultyPath The path to the JSON file containing the difficulty levels.
     * @param topicPath The path to the JSON file containing the topic definitions.
     * @return A vector of shared pointers to the loaded tasks.
     */
    QVector<QSharedPointer<Task>> loadTasks(const QString& taskPath, const QString& difficultyPath, const QString& topicPath);


private:
    using json = nlohmann::json;

    /**
     * @brief Parses tasks from JSON data.
     * 
     * @param taskJsonData The JSON data containing the tasks.
     * @param difficultyJsonData The JSON data containing the difficulty levels.
     * @param topicJsonData The JSON data containing the topic definitions.
     * @return A vector of shared pointers to the parsed tasks.
     */
    QVector<QSharedPointer<Task>> parseTasks(const json& taskJsonData, const json& difficultyJsonData, const json& topicJsonData);

    /**
     * @brief Parses subtasks from JSON data.
     * 
     * @param subtasksJson The JSON data containing the subtasks.
     * @param parentTask A shared pointer to the parent task.
     * @return A vector of parsed subtasks.
     */
    QVector<Subtask> parseSubtasks(const json& subtasksJson, QSharedPointer<Task> parentTask);
};

#endif // TASK_PARSER_HPP