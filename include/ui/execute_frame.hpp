#ifndef EXECUTE_FRAME_HPP
#define EXECUTE_FRAME_HPP

#include <QFrame>
#include <QLabel>
#include <QMovie>
#include <QHBoxLayout>

/**
 * @class ExecuteFrame
 * @brief A custom QFrame that displays an image and text. For logging and task execution.
 */
class ExecuteFrame : public QFrame
{
    Q_OBJECT

public:
    /**
     * @brief Constructor for ExecuteFrame.
     * @param parent The parent widget.
     */
    explicit ExecuteFrame(QWidget *parent = nullptr);

    /**
     * @brief Destructor for ExecuteFrame.
     */
    ~ExecuteFrame();

    /**
     * @brief Sets the image to be displayed.
     * @param path The path to the image file.
     */
    void setImage(const QString &path);

    /**
     * @brief Sets the text to be displayed.
     * @param text The text to display.
     */
    void setText(const QString &text);

    /**
     * @brief Gets the current text being displayed.
     * @return The current text.
     */
    QString getText();

private:
    QLabel *imageLabel; ///< Label to display the image.
    QLabel *textLabel; ///< Label to display the text.
    QMovie *currentMovie; ///< Movie object for animated images.
    QHBoxLayout *hLayout; ///< Layout for arranging the labels.

    /**
     * @brief Initializes the user interface.
     */
    void initializeUI();
};

#endif // EXECUTE_FRAME_HPP