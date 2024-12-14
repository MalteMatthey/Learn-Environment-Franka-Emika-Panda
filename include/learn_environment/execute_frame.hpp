#ifndef EXECUTE_FRAME_HPP
#define EXECUTE_FRAME_HPP

#include <QFrame>
#include <QLabel>
#include <QMovie>
#include <QHBoxLayout>

class ExecuteFrame : public QFrame
{
    Q_OBJECT

public:
    explicit ExecuteFrame(QWidget *parent = nullptr);
    ~ExecuteFrame();

    void setImage(const QString &path);
    void setText(const QString &text);
    QString getText();

private:
    QLabel *imageLabel;
    QLabel *textLabel;
    QMovie *currentMovie;
    QHBoxLayout *hLayout;

    void initializeUI();
};

#endif // EXECUTE_FRAME_HPP
