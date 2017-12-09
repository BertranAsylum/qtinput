#include "drag-controller.h"

#include <QDebug>
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <math.h>

#include "drag-event-notifier.h"
#include "linear-overlay.h"
#include "circular-overlay.h"

using namespace QtInputTools;

void DragController::attach(QWidget *controlled, const Mode &mode, double sensitivity) {
    new DragController(controlled, mode, sensitivity);
}

DragController::DragController(QWidget *controlled, const Mode &mode, double sensitivity)
    : QObject(static_cast<QObject*>(controlled)),
      mMode(mode),
      mSensitivity(sensitivity),
      mLastPressedValue(0.),
      mLastAngle(-1.),
      mTotalAngle(0.),
      RADIUS_THRESHOLD(50.) {
    setupController(controlled);
}

void DragController::setupController(QWidget *controlled) {
    if(dynamic_cast<QAbstractSpinBox*>(controlled)) {
        if(QLineEdit *lineEdit = controlled->findChild<QLineEdit*>()) {
            DragEventNotifier *dragEventNotifier = new DragEventNotifier(controlled);
            connect(dragEventNotifier, SIGNAL(pressed()), this, SLOT(onPressed()));
            connect(dragEventNotifier, SIGNAL(released()), this, SLOT(onReleased()));
            connect(dragEventNotifier, SIGNAL(dragged(QPoint)), this, SLOT(onDragged(QPoint)));

            switch(mMode) {
                case LINEAR:
                    connect(dragEventNotifier, SIGNAL(pressed()), this, SLOT(showLinearOverlay()));
                    break;
                case CIRCULAR:
                case CIRCULAR_SYM:
                case CIRCULAR_INF:
                    connect(dragEventNotifier, SIGNAL(pressed()), this, SLOT(showCircularOverlay()));
                    break;
            }

            lineEdit->installEventFilter(dragEventNotifier);
        }
    } else {
        qCritical() << Q_FUNC_INFO << "Unsupported widget" << controlled;
    }
}

double DragController::linearValue(const QPoint &offset) {
    return mLastPressedValue - offset.y() * mSensitivity;
}

double DragController::circularValue(const QPoint &offset, double min, double max) {
    double angle = atan2(offset.x(), -offset.y());
    if(angle < 0.) {
        angle += 2*M_PI;
    }
    angle = gluedAngle(angle, QList<double>() << 0. << M_PI_2 << M_PI << 3*M_PI_2 << 2*M_PI);
    return (angle/(2*M_PI)) * (max - min) + min;
}

double DragController::circularSymValue(const QPoint &offset, double min, double max) {
    double angle = atan2(offset.x(), -offset.y());
    angle = gluedAngle(angle, QList<double>() << 0. << M_PI_2 << M_PI << -M_PI_2 << -M_PI);
    return ((angle + M_PI)/(2*M_PI)) * (max - min) + min;
}

double DragController::circularInfValue(const QPoint &offset) {
    double angle = atan2(offset.x(), -offset.y());
    if(angle < 0.) {
        angle += 2*M_PI;
    }
    if(mLastAngle < 0. && offset.manhattanLength() < RADIUS_THRESHOLD) {
        return mLastPressedValue;
    } else if(mLastAngle < 0.) {
        mLastAngle = angle;
    }
    while(angle - mLastAngle < -M_PI) {
        angle += 2*M_PI;
    }
    while(angle - mLastAngle > M_PI) {
        angle -= 2*M_PI;
    }
    mTotalAngle += angle - mLastAngle;
    mLastAngle = angle;
    return mLastPressedValue + mTotalAngle*(180./M_PI) * mSensitivity;
}

void DragController::getRange(QObject *controlled, double *min, double *max) {
    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        if(min) *min = spinBox->minimum();
        if(max) *max = spinBox->maximum();
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        if(min) *min = spinBox->minimum();
        if(max) *max = spinBox->maximum();
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        if(min) *min = spinBox->minimumTime().msecsSinceStartOfDay();
        if(max) *max = spinBox->maximumTime().msecsSinceStartOfDay();
    }
}

double DragController::calcValue(const QPoint &offset, double min, double max) {
    switch(mMode) {
        case LINEAR: return linearValue(offset);
        case CIRCULAR: return circularValue(offset, min, max);
        case CIRCULAR_SYM: return circularSymValue(offset, min, max);
        case CIRCULAR_INF: return circularInfValue(offset);
    }
    return 0.;
}

void DragController::setValue(QObject *controlled, const QPoint &offset) {
    double min = 0., max = 0.;
    getRange(sender()->parent(), &min, &max);
    double value = calcValue(offset, min, max);

    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        spinBox->setValue(qRound(value));
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        spinBox->setValue(value);
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        spinBox->setTime(QTime::fromMSecsSinceStartOfDay(value * 1000.));
    }
}

double DragController::gluedAngle(double currentAngle, const QList<double> &anchorAngles) {
    double threshold = 0.2;
    foreach (double anchorAngle, anchorAngles) {
        if(currentAngle > anchorAngle - threshold &&
           currentAngle < anchorAngle + threshold) {
            return anchorAngle;
        }
    }
    return currentAngle;
}

void DragController::onPressed() {
    QObject *controlled = sender()->parent();
    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        mLastPressedValue = spinBox->value();
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        mLastPressedValue = spinBox->value();
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        mLastPressedValue = spinBox->time().msecsSinceStartOfDay() / 1000.;
    }
}

void DragController::onReleased() {
    mLastAngle = -1.;
    mTotalAngle = 0.;
}

void DragController::onDragged(const QPoint &offset) {
    setValue(sender()->parent(), offset);
}

void DragController::showLinearOverlay() {
    DragEventNotifier *dragEventNotifier = static_cast<DragEventNotifier*>(sender());
    QWidget *controlled = static_cast<QWidget*>(dragEventNotifier->parent());
    LinearOverlay *overlay = new LinearOverlay(static_cast<QWidget*>(controlled->parent()));
    overlay->setGeometry(controlled->x() - 10,
                         controlled->y() - controlled->width()/2. - 10. + controlled->height()/2.,
                         controlled->width() + 20.,
                         controlled->width() + 20.);
    connect(dragEventNotifier, SIGNAL(released()), overlay, SLOT(close()));
    overlay->show();
}

void DragController::showCircularOverlay() {
    DragEventNotifier *dragEventNotifier = static_cast<DragEventNotifier*>(sender());
    QWidget *controlled = static_cast<QWidget*>(dragEventNotifier->parent());
    CircularOverlay *overlay = new CircularOverlay(static_cast<QWidget*>(controlled->parent()));
    overlay->setGeometry(controlled->x() - 10,
                         controlled->y() - controlled->width()/2. - 10. + controlled->height()/2.,
                         controlled->width() + 20.,
                         controlled->width() + 20.);
    connect(dragEventNotifier, SIGNAL(dragged(QPoint)), overlay, SLOT(setOffset(QPoint)));
    connect(dragEventNotifier, SIGNAL(dragged(QPoint)), overlay, SLOT(update()));
    connect(dragEventNotifier, SIGNAL(released()), overlay, SLOT(close()));
    overlay->show();
}
