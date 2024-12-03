#ifndef TASK_H
#define TASK_H

#include <string>
#include <vector>

struct Subtask {
    std::string title;
    std::string description;
    std::string file;
    std::string difficulty;
    std::string solution_file;
    std::string evaluation_file;
};

struct Task {
    std::string title;
    std::string description;
    std::string folder;
    std::string difficulty;
    std::vector<Subtask> subtasks;
};

#endif // TASK_H
