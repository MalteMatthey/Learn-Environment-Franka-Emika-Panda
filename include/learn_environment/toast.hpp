#ifndef TOAST_HPP
#define TOAST_HPP

#include <QWidget>
#include <QLabel>
#include <QTimer>

class Toast : public QWidget {
    Q_OBJECT

public:
    explicit Toast(QWidget *parent = nullptr);
    void showToast(const QString &message);

private Q_SLOTS:
    void fadeOut();

private:
    QLabel *toastLabel;
    QTimer *toastTimer;
};

#endif // TOAST_HPP