#ifndef DRAGCONTROLLER_H
#define DRAGCONTROLLER_H

#include <QObject>
#include <QList>

namespace QtInputTools {

class DragController : public QObject {
    Q_OBJECT
public:
    enum Mode { LINEAR, CIRCULAR, CIRCULAR_SYM, CIRCULAR_INF };

private:
    Mode mMode;
    double mSensitivity;
    double mLastPressedValue;
    double mLastAngle;
    double mTotalAngle;

    const double RADIUS_THRESHOLD;

public:
    static void attach(QWidget *controlled, const Mode &mode, double sensitivity = 1.);

private:
    explicit DragController(QWidget *controlled, const Mode &mode, double sensitivity);

    void setupController(QWidget *controlled);

    double linearValue(const QPoint &offset);
    double circularValue(const QPoint &offset, double min, double max);
    double circularSymValue(const QPoint &offset, double min, double max);
    double circularInfValue(const QPoint &offset);

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

#endif // DRAGCONTROLLER_H
