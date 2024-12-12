#ifndef CUSTOM_LIST_WIDGET_HPP
#define CUSTOM_LIST_WIDGET_HPP

#include <QListWidget>
#include <QMouseEvent>
#include <QApplication>

/**
 * @class CustomListWidget
 * @brief A custom QListWidget with enhanced mouse interaction.
 *
 * The CustomListWidget class provides a custom implementation of QListWidget
 * with additional functionalities such as custom mouse cursor handling and
 * specific styling.
 */
class CustomListWidget : public QListWidget {
    Q_OBJECT

public:
    /**
     * @brief Constructs a CustomListWidget object and formats it correctly.
     * @param parent Pointer to the parent widget.
     */
    explicit CustomListWidget(QWidget *parent = nullptr);

protected:
    /**
     * @brief Handles the mouse move event.
     * @param event Pointer to the QMouseEvent object.
     */
    void mouseMoveEvent(QMouseEvent *event) override;

    /**
     * @brief Handles the leave event.
     * @param event Pointer to the QEvent object.
     */
    void leaveEvent(QEvent *event) override;

private:
    void formatListWidget();

};

#endif // CUSTOM_LIST_WIDGET_HPP