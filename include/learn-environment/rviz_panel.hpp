#ifndef LEARN_WINDOW_H
#define LEARN_WINDOW_H

#include <rviz/panel.h>
#include <ui_learn_panel.h>

#include <QWidget>
#include <QObject>
#include <QProcess>


QT_BEGIN_NAMESPACE
namespace Ui {
class Learn_Window;
}
QT_END_NAMESPACE

class Learn_Window : public rviz::Panel {
    Q_OBJECT

public:
    Learn_Window(QWidget *parent = 0);  // Constructor, default parent widget
    ~Learn_Window();                         // Destructor

    // Save and load user-specific settings
    virtual void load(const rviz::Config &config) override;
    virtual void save(rviz::Config config) const override;

public Q_SLOTS:
    void onExecuteButtonClicked();            // Slot for button click
    void checkResult();

private:
    Ui::Learn_Window *ui;                // Pointer to UI elements
    QProcess *process;                 
};

#endif // LEARN_WINDOW_H