#ifndef TASKUI_HPP
#define TASKUI_HPP

#include "ui/sidebar.hpp"
#include "common/task.hpp"
#include "task_management/task_manager.hpp"
#include "ui/execute_frame.hpp"

#include <QObject>
#include <QVBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QPushButton>
#include <QFrame>

/**
 * @class TaskUI
 * @brief Manages the user interface for tasks and subtasks.
 *
 * The TaskUI class is responsible for initializing and updating the UI elements
 * related to tasks and subtasks. It interacts with the TaskManager and Sidebar
 * to provide a cohesive user experience.
 */
class TaskUI : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a TaskUI object.
     * @param subtaskListLayout Pointer to the vertical layout for displaying subtasks.
     * @param mainTitleLabel Pointer to the QLabel for displaying the main title.
     * @param difficultyLabel Pointer to the QLabel for displaying the difficulty level.
     * @param folderLabel Pointer to the QLabel for displaying the folder information.
     * @param nextButton Pointer to the QPushButton for navigating to the next task.
     * @param previousButton Pointer to the QPushButton for navigating to the previous task.
     * @param menuButton Pointer to the QToolButton for opening the sidebar.
     * @param resetRobotStartButton Pointer to the QToolButton for resetting the robot.
     * @param resetRobotFrame Pointer to the QFrame displaying the reset of the robot.
     * @param centralWidget Pointer to the central QWidget for the plugin.
     * @param parent Pointer to the parent QObject.
     */
    TaskUI(QVBoxLayout *subtaskListLayout, QLabel *mainTitleLabel,
           QLabel *difficultyLabel, QLabel *folderLabel, QLabel *topicLabel,
           QPushButton *nextButton, QPushButton *previousButton, QToolButton *menuButton,
           QToolButton *resetRobotStartButton, QFrame *resetRobotFrame, QWidget *centralwidget, QWidget *parent = nullptr);

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

    /**
     * @brief Update the UI to show that the robot reset has been started.
     * @param noSubtasksLeft indicator, if subtasks are running after the rest or not.
     */
    void startedRobotResetUI(bool noSubtasksLeft);

    /**
     * @brief Update the UI to show that the robot reset has finished.
     */
    void finishedRobotResetUI();

    /**
     * @brief Update the UI to show that the robot reset has failed.
     * @param error the error that occured while resetting the robot.
     */
    void failedRobotResetUI(const QString &error);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a task is selected.
     * @param index Index of the selected task.
     */
    void taskSelected(int index);

private Q_SLOTS:
    /**
     * @brief Toggles the visibility of the sidebar.
     */
    void toggleSidebarVisibility();

private:
    Sidebar *sidebar; ///< Pointer to the Sidebar object.
    QVBoxLayout *subtaskListLayout; ///< Pointer to the QVBoxLayout for displaying subtasks.
    QLabel *mainTitleLabel; ///< Pointer to the QLabel for displaying the main title.
    QLabel *difficultyLabel; ///< Pointer to the QLabel for displaying the difficulty level.
    QLabel *folderLabel; ///< Pointer to the QLabel for displaying the folder information.
    QLabel *topicLabel; ///< Pointer to the QLabel for displaying the topic of the task.
    QPushButton *nextButton; ///< Pointer to the QPushButton for navigating to the next task.
    QPushButton *previousButton; ///< Pointer to the QPushButton for navigating to the previous task.
    QToolButton *menuButton; ///< Pointer to the QToolButton for opening the sidebar.
    QToolButton *resetRobotStartButton; ///< Pointer to the QToolButton for resetting the robot.
    QFrame *resetRobotFrame; ///< Pointer to the reset robot frame.
    ExecuteFrame *executeResetRobotFrame; ///< Pointer to the execute reset robot frame.
    QWidget *centralwidget; ///< Pointer to the central QWidget for the plugin.
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
    
    /**
     * @brief Sets up the splitter and layout for the main window.
     */
    void setupSplitterAndLayout();
};

#endif // TASKUI_HPP