#ifndef QTINPUT_DRAGCONTROLLER_H
#define QTINPUT_DRAGCONTROLLER_H

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
    static void attach(QWidget * controlled,
                       qreal sensitivity = 1.0);
    static void attach(QWidget * controlled,
                       qreal minPos,
                       qreal maxPos);
    static void attach(QWidget * controlled,
                       bool clockwise,
                       qreal sensitivity = 1.0);

private:
    DragController(QWidget * controlled,
                   qreal sensitivity);
    DragController(QWidget * controlled,
                   qreal minPos,
                   qreal maxPos);
    DragController(QWidget * controlled,
                   bool clockwise,
                   qreal sensitivity);

private:
    Q_DECLARE_PRIVATE(DragController)
    QScopedPointer<DragControllerPrivate> dptr;
};

}

#endif // QTINPUT_DRAGCONTROLLER_H
