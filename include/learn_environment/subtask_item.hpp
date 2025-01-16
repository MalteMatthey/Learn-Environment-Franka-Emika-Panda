#ifndef SUBTASK_ITEM_HPP
#define SUBTASK_ITEM_HPP

#include "task.hpp"
#include "task_manager.hpp"
#include "execute_frame.hpp"
#include "toast.hpp"

#include <QWidget>
#include <QPushButton>
#include <QToolButton>
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
    void updateUI(bool constructorCall = false);

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
    void handleHelpButtonClick();
    void handleStartOwnScript();
    void handleStartSolution();
    void handleToggleSolution();
    void handleResetNotebook(); 

protected:
    void resizeEvent(QResizeEvent* event) override;  

private:
    /**
     * @brief Sets up the UI elements for the subtask item.
     * @param headerText The header text for the subtask.
     * @param linkText The link text for the subtask.
     * @param bodyText The body text for the subtask.
     */
    void setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText);
    void initializeHelpMenu();
    void initializeStartMenu();
    void setExecutionFrame(const QString& imagePath, const QString& text);
    void showToast(const QString &message); ///< Displays a toast message.

    TaskManager *taskManager; ///< Pointer to the TaskManager object.
    Subtask *subtask; ///< Pointer to the subtask object.
    QString headerText; ///< The header text for the subtask.
    QString linkText; ///< The link text for the subtask.
    QString bodyText; ///< The body text for the subtask.
    QToolButton *startButton; ///< Button to start/stop the subtask.
    QToolButton *helpButton; ///< Button to show the help menu for the subtask.
    QVBoxLayout *baseLayout; ///< Layout for the subtask item.
    ExecuteFrame *executeSubtaskFrame; ///< Pointer to the execute subtask frame.

    QPushButton *menuStartOwnBtn; ///< Button to start the user's script.
    QPushButton *menuStartSolutionBtn; ///< Button to start the solution.

    QPushButton *menuToggleSolutionBtn; ///< Button to toggle the solution.
    QPushButton *menuResetNotebookBtn; ///< Button to reset the notebook.

    QMenu *startMenu; ///< Menu for starting the subtask.
    QMenu *helpMenu; ///< Menu for showing the help options.

    Toast *toast; ///< Toast message for subtask.
};

#endif // SUBTASK_ITEM_HPP