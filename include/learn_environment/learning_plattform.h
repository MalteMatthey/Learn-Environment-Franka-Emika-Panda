#ifndef LEARNING_PLATTFORM_H
#define LEARNING_PLATTFORM_H

#include <rviz/panel.h>
#include <ui_learning_plattform.h>
#include "sidebar.h"
#include "task_manager.h"
#include "task_ui.h"

#include <QWidget>
#include <QObject>
#include <QProcess>

QT_BEGIN_NAMESPACE
namespace Ui {
    class LearningPlattform;
}
QT_END_NAMESPACE

class LearningPlattform : public rviz::Panel
{
    Q_OBJECT

public:
    LearningPlattform(QWidget *parent = 0);
    ~LearningPlattform();

    // Save and load user-specific settings
    virtual void load(const rviz::Config &config) override;
    virtual void save(rviz::Config config) const override;

private Q_SLOTS:
    void toggleSidebarVisibility();

private:
    Ui::LearningPlattform *ui;
    Sidebar *sidebar;
    TaskManager *taskManager;
    TaskUI *taskUI;
    QProcess *process;

    void initializeUI();
    void setupSplitterAndLayout();
};

#endif // LEARNING_PLATTFORM_H