#ifndef DRAGCONTROLLER_H
#define DRAGCONTROLLER_H

#include <QObject>
#include <QScopedPointer>

namespace qtinput {

class DragControllerPrivate;
class DragController : public QObject {
    enum Type {
        Linear,
        Circular,
        CircularInf,
        CircularInfCw
    };

public:
    static void attach(QWidget *controlled,
                       double sensitivity = 1.0);
    static void attach(QWidget *controlled,
                       double minPos,
                       double maxPos);
    static void attach(QWidget *controlled,
                       bool clockwise,
                       double sensitivity = 1.0);

private:
    DragController(QWidget *controlled,
                   double sensitivity);
    DragController(QWidget *controlled,
                   double minPos,
                   double maxPos);
    DragController(QWidget *controlled,
                   bool clockwise,
                   double sensitivity);

private:
    Q_DECLARE_PRIVATE(DragController)
    QScopedPointer<DragControllerPrivate> d_ptr;
};

}

#endif // DRAGCONTROLLER_H
