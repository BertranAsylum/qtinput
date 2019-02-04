#ifndef DRAGCONTROLLERPRIVATE
#define DRAGCONTROLLERPRIVATE

#include <QList>
#include <math.h>

#include "drag-controller.h"

class QRect;
class QPoint;

namespace qtinput {

class DragControllerPrivate : public QObject {
    Q_OBJECT

public:
    DragController::Type type = DragController::CircularInfCw;
    qreal sensitivity = 1.0;
    qreal minPos = 0.0;
    qreal maxPos = 2.0*M_PI;
    qreal lastPressedValue = 0.0;
    qreal lastAngle = -1.0;
    qreal totalAngle = 0.0;

public:
    DragControllerPrivate();

public:
    void setupController(QWidget * controlled);

private:
    QRect overlayGeometry(QWidget * controlled);

    qreal linearValue(const QPoint & offset);
    qreal circularValue(const QPoint & offset, qreal min, qreal max);
    qreal circularInfValue(const QPoint & offset, bool clockwise);

    void getRange(QObject * controlled, qreal * min, qreal * max);
    qreal calcValue(const QPoint & offset, qreal min, qreal max);
    void setValue(QObject * controlled, const QPoint & offset);
    qreal gluedAngle(qreal currentAngle, const QList<qreal> & anchorAngles);

private slots:
    void onPressed();
    void onReleased();
    void onDragged(const QPoint & offset);

    void showLinearOverlay();
    void showCircularOverlay();
};

}

#endif // DRAGCONTROLLERPRIVATE

