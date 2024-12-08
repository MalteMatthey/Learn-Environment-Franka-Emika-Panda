#ifndef learn_environment_H
#define learn_environment_H

#include <rviz/panel.h>
#include <ui_learn_environment.h>
#include "sidebar.h"
#include "task_manager.h"
#include "task_ui.h"

#include <QWidget>
#include <QObject>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
    class LearnEnvironment;
}
QT_END_NAMESPACE

/**
 * @class LearnEnvironment
 * @brief A custom RViz panel for managing the learning environment.
 *
 * The LearnEnvironment class provides a user interface for managing tasks and their execution
 * and evaluation within the learning environment.
 */
class LearnEnvironment : public rviz::Panel
{
    Q_OBJECT

public:
    /**
     * @brief Constructs a LearnEnvironment object.
     * @param parent Pointer to the parent widget.
     */
    LearnEnvironment(QWidget *parent = 0);

    /**
     * @brief Destroys the LearnEnvironment object.
     */
    ~LearnEnvironment();

    /**
     * @brief Loads user-specific settings.
     * @param config The RViz configuration object.
     */
    virtual void load(const rviz::Config &config) override;

    /**
     * @brief Saves user-specific settings.
     * @param config The RViz configuration object.
     */
    virtual void save(rviz::Config config) const override;

private Q_SLOTS:
    /**
     * @brief Toggles the visibility of the sidebar.
     */
    void toggleSidebarVisibility();

private:
    Ui::LearnEnvironment *ui; ///< Pointer to the UI object.
    Sidebar *sidebar; ///< Pointer to the Sidebar object.
    TaskManager *taskManager; ///< Pointer to the TaskManager object.
    TaskUI *taskUI; ///< Pointer to the TaskUI object.
    QProcess *process; ///< Pointer to the QProcess object.

    /**
     * @brief Initializes the user interface.
     */
    void initializeUI();

    /**
     * @brief Sets up the splitter and layout for the main window.
     */
    void setupSplitterAndLayout();
};

#endif // learn_environment_H