#include "drag-controller-private.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <math.h>

#include "mouse-event-notifier.h"
#include "linear-overlay.h"
#include "circular-overlay.h"

#define RADIUS_THRESHOLD 50.0
#define RAD(deg) (deg*M_PI/180.0)
#define DEG(rad) (rad*180.0/M_PI)
#define ADJUST(rad) while(rad < 0.0) { rad += 2.0*M_PI; } \
                    while(rad > 2.0*M_PI) { rad -= 2.0*M_PI; }
#define BOUND(min,rad,max) if(rad < min || rad > max) { \
                               double minDist = qAbs(rad-min); if(minDist > M_PI) { minDist = 2.0*M_PI - minDist; } \
                               double maxDist = qAbs(rad-max); if(maxDist > M_PI) { maxDist = 2.0*M_PI - maxDist; } \
                               if(minDist < maxDist) { rad = min; } else { rad = max; } \
                           }
#define NORMALIZED(value, min, max) ((value-min) / (max-min))

using namespace qtinput;

void DragController::attach(QWidget *controlled, double sensitivity) {
    new DragController(controlled,
                       sensitivity);
}

void DragController::attach(QWidget *controlled, double minPos, double maxPos) {
    new DragController(controlled,
                       minPos,
                       maxPos);
}

void DragController::attach(QWidget *controlled, bool clockwise, double sensitivity) {
    new DragController(controlled,
                       clockwise,
                       sensitivity);
}

DragController::DragController(QWidget *controlled, double sensitivity)
    : QObject(static_cast<QObject*>(controlled)),
      d_ptr(new DragControllerPrivate()) {
    d_ptr->type = Linear;
    d_ptr->sensitivity = sensitivity;
    d_ptr->setupController(controlled);
}

DragController::DragController(QWidget *controlled, double minPos, double maxPos)
    : QObject(static_cast<QObject*>(controlled)),
      d_ptr(new DragControllerPrivate()) {
    d_ptr->type = Circular;
    if(!qFuzzyCompare(minPos, maxPos)) {
        d_ptr->minPos = RAD(minPos);
        d_ptr->maxPos = RAD(maxPos);
    }
    d_ptr->setupController(controlled);
}

DragController::DragController(QWidget *controlled, bool clockwise, double sensitivity)
    : QObject(static_cast<QObject*>(controlled)),
      d_ptr(new DragControllerPrivate()) {
    d_ptr->type = clockwise ? CircularInfCw : CircularInf;
    d_ptr->sensitivity = sensitivity;
    d_ptr->setupController(controlled);
}

DragControllerPrivate::DragControllerPrivate() {}

void DragControllerPrivate::setupController(QWidget *controlled) {
    if(dynamic_cast<QAbstractSpinBox*>(controlled)) {
        if(QLineEdit *lineEdit = controlled->findChild<QLineEdit*>()) {
            MouseEventNotifier *mouseEventNotifier = new MouseEventNotifier(controlled);
            connect(mouseEventNotifier, SIGNAL(pressed()), this, SLOT(onPressed()));
            connect(mouseEventNotifier, SIGNAL(released()), this, SLOT(onReleased()));
            connect(mouseEventNotifier, SIGNAL(dragged(QPoint)), this, SLOT(onDragged(QPoint)));

            switch(type) {
                case DragController::Linear:
                    connect(mouseEventNotifier, SIGNAL(pressed()), this, SLOT(showLinearOverlay()));
                    break;
                case DragController::Circular:
                case DragController::CircularInf:
                case DragController::CircularInfCw:
                    connect(mouseEventNotifier, SIGNAL(pressed()), this, SLOT(showCircularOverlay()));
                    break;
            }

            lineEdit->installEventFilter(mouseEventNotifier);
        }
    } else {
        qCritical() << Q_FUNC_INFO << "Unsupported widget" << controlled;
    }
}

QRect DragControllerPrivate::overlayGeometry(QWidget *controlled) {
    QPoint overlayPos;
    QSize overlaySize;
    switch(type) {
        case DragController::Linear:
            overlayPos = controlled->mapTo(QApplication::activeWindow(),
                                           QPoint(0, -controlled->height()*2));
            overlaySize = QSize(controlled->width(),
                                controlled->height()*5);
            break;
        case DragController::Circular:
        case DragController::CircularInf:
        case DragController::CircularInfCw:
            overlayPos = controlled->mapTo(QApplication::activeWindow(),
                                           QPoint(-15, -controlled->width()/2-15 + controlled->height()/2));
            overlaySize = QSize(controlled->width() + 30,
                                controlled->width() + 30);
            break;
    }
    return QRect(overlayPos, overlaySize);
}

double DragControllerPrivate::linearValue(const QPoint &offset) {
    return lastPressedValue - offset.y() * sensitivity;
}

double DragControllerPrivate::circularValue(const QPoint &offset, double min, double max) {
    double angle = atan2(-offset.y(), offset.x());
    ADJUST(angle)

    double adjustedMin = qMin(minPos, maxPos);
    double adjustedMax = qMax(minPos, maxPos);
    ADJUST(adjustedMin)
    ADJUST(adjustedMax)
    if(!qFuzzyCompare(adjustedMin, adjustedMax)) {
        BOUND(adjustedMin, angle, adjustedMax)
    }

    if(angle > qMax(minPos, maxPos)) { angle -= 2.0*M_PI; }
    bool clockwise = minPos > maxPos;
    double progress = clockwise ? 1.0-NORMALIZED(angle, maxPos, minPos)
                                : NORMALIZED(angle, minPos, maxPos);
    return progress*(max-min) + min;
}

double DragControllerPrivate::circularInfValue(const QPoint &offset, bool clockwise) {
    if(offset.manhattanLength() < RADIUS_THRESHOLD) { return lastPressedValue; }
    double angle = atan2(-offset.y(), offset.x());
    ADJUST(angle)
    if(lastAngle < 0.0) { lastAngle = angle; }

    while(angle - lastAngle < -M_PI) { angle += 2*M_PI; }
    while(angle - lastAngle > M_PI) { angle -= 2*M_PI; }

    totalAngle += angle - lastAngle;
    lastAngle = angle;
    return lastPressedValue + DEG(totalAngle)*sensitivity * (clockwise ? -1.0 : 1.0);
}

void DragControllerPrivate::getRange(QObject *controlled, double *min, double *max) {
    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        if(min) *min = spinBox->minimum();
        if(max) *max = spinBox->maximum();
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        if(min) *min = spinBox->minimum();
        if(max) *max = spinBox->maximum();
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        if(min) *min = QTime(0,0,0,0).msecsTo(spinBox->minimumTime());
        if(max) *max = QTime(0,0,0,0).msecsTo(spinBox->maximumTime());
    } else {
        QMetaObject::invokeMethod(controlled, "minimum", Q_RETURN_ARG(double, *min));
        QMetaObject::invokeMethod(controlled, "maximum", Q_RETURN_ARG(double, *max));
    }
}

double DragControllerPrivate::calcValue(const QPoint &offset, double min, double max) {
    switch(type) {
        case DragController::Linear: return linearValue(offset);
        case DragController::Circular: return circularValue(offset, min, max);
        case DragController::CircularInf: return circularInfValue(offset, false);
        case DragController::CircularInfCw: return circularInfValue(offset, true);
    }
    return 0.0;
}

void DragControllerPrivate::setValue(QObject *controlled, const QPoint &offset) {
    double min = 0.0;
    double max = 0.0;
    getRange(sender()->parent(), &min, &max);
    double value = calcValue(offset, min, max);

    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        spinBox->setValue(qRound(value));
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        spinBox->setValue(value);
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        spinBox->setTime(QTime(0,0,0,0).addMSecs(value * 1000.0));
    } else {
        QMetaObject::invokeMethod(controlled, "setValue", Q_ARG(double, value));
    }
}

double DragControllerPrivate::gluedAngle(double currentAngle, const QList<double> &anchorAngles) {
    double threshold = 0.2;
    foreach (double anchorAngle, anchorAngles) {
        if(currentAngle > anchorAngle - threshold &&
           currentAngle < anchorAngle + threshold) {
            return anchorAngle;
        }
    }
    return currentAngle;
}

void DragControllerPrivate::onPressed() {
    QObject *controlled = sender()->parent();
    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        lastPressedValue = spinBox->value();
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        lastPressedValue = spinBox->value();
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        lastPressedValue = QTime(0,0,0,0).msecsTo(spinBox->time()) / 1000.0;
    } else {
        QMetaObject::invokeMethod(controlled, "value", Q_RETURN_ARG(double, lastPressedValue));
    }
}

void DragControllerPrivate::onReleased() {
    lastAngle = -1.0;
    totalAngle = 0.0;
    if(QLineEdit *lineEdit = sender()->parent()->findChild<QLineEdit*>()) {
        lineEdit->end(false);
    }
}

void DragControllerPrivate::onDragged(const QPoint &offset) {
    setValue(sender()->parent(), offset);
}

void DragControllerPrivate::showLinearOverlay() {
    MouseEventNotifier *mouseEventNotifier = static_cast<MouseEventNotifier*>(sender());

    LinearOverlay *overlay = new LinearOverlay(static_cast<QWidget*>(QApplication::activeWindow()));
    overlay->setGeometry(overlayGeometry(static_cast<QWidget*>(mouseEventNotifier->parent())));

    connect(mouseEventNotifier, SIGNAL(released()), overlay, SLOT(close()));
    overlay->show();
}

void DragControllerPrivate::showCircularOverlay() {
    MouseEventNotifier *mouseEventNotifier = static_cast<MouseEventNotifier*>(sender());

    CircularOverlay *overlay = new CircularOverlay(static_cast<QWidget*>(QApplication::activeWindow()));
    overlay->setGeometry(overlayGeometry(static_cast<QWidget*>(mouseEventNotifier->parent())));
    if(type == DragController::Circular) {
        double adjustedMin = qMin(minPos, maxPos);
        double adjustedMax = qMax(minPos, maxPos);
        ADJUST(adjustedMin)
        ADJUST(adjustedMax)
        overlay->setBounds(adjustedMin, adjustedMax);
    }

    connect(mouseEventNotifier, SIGNAL(dragged(QPoint)), overlay, SLOT(setOffset(QPoint)));
    connect(mouseEventNotifier, SIGNAL(dragged(QPoint)), overlay, SLOT(update()));
    connect(mouseEventNotifier, SIGNAL(released()), overlay, SLOT(close()));
    overlay->show();
}
