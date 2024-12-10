#ifndef SUBTASK_ITEM_HPP
#define SUBTASK_ITEM_HPP

#include "task.hpp"
#include "task_manager.hpp"

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

/**
 * @class SubtaskItem
 * @brief Represents a UI item for a subtask.
 *
 * The SubtaskItem class is responsible for displaying and managing the UI elements
 * related to a subtask. It provides functionalities to update the UI based on the
 * subtask's status and to handle user interactions.
 * This is a QWidget designed to function as a List Item.
 */
class SubtaskItem : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a SubtaskItem object.
     * @param parent Pointer to the parent widget.
     * @param subtask Pointer to the subtask object.
     */
    SubtaskItem(QWidget *parent, Subtask *subtask);

    /**
     * @brief Updates the UI elements based on the subtask's status.
     */
    void updateUI();

    /**
     * @brief Sets the TaskManager object.
     * @param manager Pointer to the TaskManager object.
     */
    void setTaskManager(TaskManager *manager);

Q_SIGNALS:
    /**
     * @brief Signal emitted when a start/stop request is made for the subtask.
     * @param subtask The subtask for which the request is made.
     */
    void subtaskStartStopRequested(const Subtask &subtask);

private Q_SLOTS:
    /**
     * @brief Handles the start button click event.
     */
    void handleStartButtonClick();

private:
    /**
     * @brief Sets up the UI elements for the subtask item.
     * @param headerText The header text for the subtask.
     * @param linkText The link text for the subtask.
     * @param bodyText The body text for the subtask.
     */
    void setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText);

    TaskManager *taskManager; ///< Pointer to the TaskManager object.
    Subtask *subtask; ///< Pointer to the subtask object.
    QString headerText; ///< The header text for the subtask.
    QString linkText; ///< The link text for the subtask.
    QString bodyText; ///< The body text for the subtask.
    QPushButton *playButton; ///< Button to start/stop the subtask.
    QPushButton *resetButton; ///< Button to reset the subtask.
    QPushButton *solutionButton; ///< Button to show the solution for the subtask.
};

#endif // SUBTASK_ITEM_HPP