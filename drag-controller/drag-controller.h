#ifndef DRAGCONTROLLER_H
#define DRAGCONTROLLER_H

#include <QObject>

namespace QtInputTools {

class DragController : public QObject {
    Q_OBJECT
public:
    enum Mode { LINEAR, CIRCULAR, CIRCULAR_SYM };

private:
    Mode mMode;
    double mSensitivity;
    double mLastPressedValue;

public:
    static void attach(QWidget *controlled, const Mode &mode, double sensitivity = 1.);

private:
    explicit DragController(QWidget *controlled, const Mode &mode, double sensitivity);

    void installDragEventNotifier(QWidget *controlled);
    double linearValue(const QPoint &offset);
    double circularValue(const QPoint &offset, double min, double max);
    double circularSymValue(const QPoint &offset, double min, double max);

private slots:
    void onPressed();
    void onDragged(const QPoint &offset);
};

}

#endif // DRAGCONTROLLER_H
