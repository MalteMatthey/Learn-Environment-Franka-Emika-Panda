#ifndef SUBTASK_ITEM_H
#define SUBTASK_ITEM_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QHBoxLayout>
#include <QVBoxLayout>

class SubtaskItem : public QWidget {
    Q_OBJECT

public:
    SubtaskItem(QWidget *parent = nullptr, const QString &headerText = "", const QString &linkText = "", const QString &bodyText = "");

private Q_SLOTS:
    void handleButtonClick();

private:
    void setupItemUI(const QString &headerText, const QString &linkText, const QString &bodyText);
};

#endif // SUBTASK_ITEM_H