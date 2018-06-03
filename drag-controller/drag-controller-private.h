#ifndef DRAGCONTROLLERPRIVATE
#define DRAGCONTROLLERPRIVATE

#include <QList>
#include "drag-controller.h"

class QRect;
class QPoint;

namespace qtinput {

class DragControllerPrivate : public QObject {
    Q_OBJECT
public:
    DragControllerPrivate();
    void setupController(QWidget *controlled);

public:
    DragController::Type type = DragController::CircularInfCw;
    double sensitivity = 1.0;
    double minPos = 0.0;
    double maxPos = 0.0;
    double lastPressedValue = 0.0;
    double lastAngle = -1.0;
    double totalAngle = 0.0;

private:
    QRect overlayGeometry(QWidget *controlled);

    double linearValue(const QPoint &offset);
    double circularValue(const QPoint &offset, double min, double max);
    double circularInfValue(const QPoint &offset, bool clockwise);

    void getRange(QObject *controlled, double *min, double *max);
    double calcValue(const QPoint &offset, double min, double max);
    void setValue(QObject *controlled, const QPoint &offset);
    double gluedAngle(double currentAngle, const QList<double> &anchorAngles);

private slots:
    void onPressed();
    void onReleased();
    void onDragged(const QPoint &offset);

    void showLinearOverlay();
    void showCircularOverlay();
};

}

#endif // DRAGCONTROLLERPRIVATE

