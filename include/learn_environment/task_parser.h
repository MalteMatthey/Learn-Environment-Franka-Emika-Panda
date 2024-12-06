#ifndef TASK_PARSER_H
#define TASK_PARSER_H

#include <QVector>
#include <QString>
#include <QSharedPointer>
#include "task.h"
#include <nlohmann/json.hpp>

class TaskParser {
public:
    QVector<QSharedPointer<Task>> loadTasks(const QString& filePath);

private:
    using json = nlohmann::json;
    QVector<QSharedPointer<Task>> parseTasks(const json& jsonData);
    QVector<Subtask> parseSubtasks(const json& subtasksJson, QSharedPointer<Task> parentTask);
};

#endif // TASK_PARSER_H