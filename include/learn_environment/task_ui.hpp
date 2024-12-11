#ifndef TASKUI_HPP
#define TASKUI_HPP

#include "sidebar.hpp"
#include "task.hpp"
#include "task_manager.hpp"

#include <QObject>
#include <QListWidget>
#include <QLabel>
#include <QPushButton>

/**
 * @class TaskUI
 * @brief Manages the user interface for tasks and subtasks.
 *
 * The TaskUI class is responsible for initializing and updating the UI elements
 * related to tasks and subtasks. It interacts with the TaskManager and Sidebar
 * to provide a cohesive user experience.
 */
class TaskUI : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a TaskUI object.
     * @param subtaskListWidget Pointer to the QListWidget for displaying subtasks.
     * @param mainTitleLabel Pointer to the QLabel for displaying the main title.
     * @param difficultyLabel Pointer to the QLabel for displaying the difficulty level.
     * @param folderLabel Pointer to the QLabel for displaying the folder information.
     * @param nextButton Pointer to the QPushButton for navigating to the next task.
     * @param previousButton Pointer to the QPushButton for navigating to the previous task.
     * @param sidebar Reference to the Sidebar object.
     * @param parent Pointer to the parent QObject.
     */
    TaskUI(QListWidget *subtaskListWidget, QLabel *mainTitleLabel,
           QLabel *difficultyLabel, QLabel *folderLabel,
           QPushButton *nextButton, QPushButton *previousButton,
           Sidebar &sidebar, QObject *parent = nullptr);

    /**
     * @brief Initializes the UI with the loaded tasks.
     * @param tasks Vector of loaded tasks.
     */
    void initializeUI(const QVector<QSharedPointer<Task>> &tasks);

    /**
     * @brief Sets the UI elements for the current task.
     * @param currentTaskIndex Index of the current task.
     */
    void setTaskUI(int currentTaskIndex);

    /**
     * @brief Updates the UI elements for the subtasks.
     */
    void updateSubtaskItemsUI();

    /**
     * @brief Sets the TaskManager object.
     * @param manager Pointer to the TaskManager object.
     */
    void setTaskManager(TaskManager *manager);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a task is selected.
     * @param index Index of the selected task.
     */
    void taskSelected(int index);

private:
    Sidebar &sidebar; ///< Reference to the Sidebar object.
    QListWidget *subtaskListWidget; ///< Pointer to the QListWidget for displaying subtasks.
    QLabel *mainTitleLabel; ///< Pointer to the QLabel for displaying the main title.
    QLabel *difficultyLabel; ///< Pointer to the QLabel for displaying the difficulty level.
    QLabel *folderLabel; ///< Pointer to the QLabel for displaying the folder information.
    QPushButton *nextButton; ///< Pointer to the QPushButton for navigating to the next task.
    QPushButton *previousButton; ///< Pointer to the QPushButton for navigating to the previous task.
    QVector<QSharedPointer<Task>> tasks; ///< Vector of loaded tasks.
    TaskManager *taskManager; ///< Pointer to the TaskManager object.

    /**
     * @brief Sets the subtask items in the QListWidget.
     * @param currentTaskIndex Index of the current task.
     */
    void setSubtaskItems(int currentTaskIndex);

    /**
     * @brief Adds a line between subtask widgets in the QListWidget.
     */
    void addLineBetweenWidgets();
};

#endif // TASKUI_HPP