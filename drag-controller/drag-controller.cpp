#include "drag-controller-private.h"

#include <QDebug>
#include <QApplication>
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>

#include "mouse-event-notifier.h"
#include "linear-overlay.h"
#include "circular-overlay.h"

#define RADIUS_THRESHOLD 50.0
#define RAD(deg) (deg*M_PI/180.0)
#define DEG(rad) (rad*180.0/M_PI)

#define NORMALIZE(rad) rad = fmod(rad, 2.0*M_PI); \
                       if (rad < 0.0) { rad += 2.0*M_PI; }

#define BOUND(min,rad,max) if(rad < min || rad > max) { \
                               qreal minDist = qAbs(rad-min); if(minDist > M_PI) { minDist = 2.0*M_PI - minDist; } \
                               qreal maxDist = qAbs(rad-max); if(maxDist > M_PI) { maxDist = 2.0*M_PI - maxDist; } \
                               if(minDist < maxDist) { rad = min; } else { rad = max; } \
                           }
#define NORMALIZED(value, min, max) ((value-min) / (max-min))

namespace qtinput {

void DragController::attach(QWidget * controlled, qreal sensitivity) {
    new DragController(controlled,
                       sensitivity);
}

void DragController::attach(QWidget * controlled, qreal minPos, qreal maxPos) {
    new DragController(controlled,
                       minPos,
                       maxPos);
}

void DragController::attach(QWidget * controlled, bool clockwise, qreal sensitivity) {
    new DragController(controlled,
                       clockwise,
                       sensitivity);
}

DragController::DragController(QWidget * controlled, qreal sensitivity) :
    QObject(static_cast<QObject *>(controlled)),
    dptr(new DragControllerPrivate())
{
    dptr->type = Linear;
    dptr->sensitivity = sensitivity;
    dptr->setupController(controlled);
}

DragController::DragController(QWidget * controlled, qreal minPos, qreal maxPos) :
    QObject(static_cast<QObject *>(controlled)),
    dptr(new DragControllerPrivate())
{
    dptr->type = Circular;
    if (!qFuzzyCompare(minPos, maxPos)) {
        dptr->minPos = RAD(minPos);
        dptr->maxPos = RAD(maxPos);
    }
    dptr->setupController(controlled);
}

DragController::DragController(QWidget * controlled, bool clockwise, qreal sensitivity) :
    QObject(static_cast<QObject *>(controlled)),
    dptr(new DragControllerPrivate())
{
    dptr->type = clockwise ? CircularInfCw : CircularInf;
    dptr->sensitivity = sensitivity;
    dptr->setupController(controlled);
}

DragControllerPrivate::DragControllerPrivate()
{}

void DragControllerPrivate::setupController(QWidget * controlled)
{
    if (dynamic_cast<QAbstractSpinBox *>(controlled))
    {
        if (QLineEdit * lineEdit = controlled->findChild<QLineEdit *>())
        {
            MouseEventNotifier * mouseEventNotifier = new MouseEventNotifier(controlled);
            connect(mouseEventNotifier, &MouseEventNotifier::pressed, this, &DragControllerPrivate::onPressed);
            connect(mouseEventNotifier, &MouseEventNotifier::released, this, &DragControllerPrivate::onReleased);
            connect(mouseEventNotifier, &MouseEventNotifier::dragged, this, &DragControllerPrivate::onDragged);

            switch (type) {
                case DragController::Linear:
                    connect(mouseEventNotifier, &MouseEventNotifier::pressed, this, &DragControllerPrivate::showLinearOverlay);
                    break;
                case DragController::Circular:
                case DragController::CircularInf:
                case DragController::CircularInfCw:
                    connect(mouseEventNotifier, &MouseEventNotifier::pressed, this, &DragControllerPrivate::showCircularOverlay);
                    break;
            }

            lineEdit->installEventFilter(mouseEventNotifier);
        }
    }
    else
    {
        qCritical() << Q_FUNC_INFO << "Unsupported widget" << controlled;
    }
}

QRect DragControllerPrivate::overlayGeometry(QWidget * controlled)
{
    QPoint overlayPos;
    QSize overlaySize;

    switch (type)
    {
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

qreal DragControllerPrivate::linearValue(const QPoint & offset)
{
    return lastPressedValue - offset.y() * sensitivity;
}

qreal DragControllerPrivate::circularValue(const QPoint & offset, qreal min, qreal max)
{
    qreal angle = atan2(-offset.y(), offset.x());
    NORMALIZE(angle)

    qreal adjustedMin = qMin(minPos, maxPos);
    qreal adjustedMax = qMax(minPos, maxPos);

    NORMALIZE(adjustedMin)
    NORMALIZE(adjustedMax)

    if (!qFuzzyCompare(adjustedMin, adjustedMax)) {
        BOUND(adjustedMin, angle, adjustedMax)
    }

    if (angle > qMax(minPos, maxPos)) { angle -= 2.0*M_PI; }

    bool clockwise = minPos > maxPos;
    qreal progress = clockwise ? 1.0-NORMALIZED(angle, maxPos, minPos)
                               : NORMALIZED(angle, minPos, maxPos);

    return progress*(max-min) + min;
}

qreal DragControllerPrivate::circularInfValue(const QPoint & offset, bool clockwise)
{
    if (offset.manhattanLength() < RADIUS_THRESHOLD) {
        return lastPressedValue;
    }

    qreal angle = atan2(-offset.y(), offset.x());

    NORMALIZE(angle)

    if (lastAngle < 0.0) {
        lastAngle = angle;
    }

    while (angle - lastAngle < -M_PI) { angle += 2*M_PI; }
    while (angle - lastAngle > M_PI) { angle -= 2*M_PI; }

    totalAngle += angle - lastAngle;
    lastAngle = angle;

    return lastPressedValue + DEG(totalAngle)*sensitivity * (clockwise ? -1.0 : 1.0);
}

void DragControllerPrivate::getRange(QObject * controlled, qreal * min, qreal * max)
{
    if (QSpinBox * spinBox = dynamic_cast<QSpinBox *>(controlled)) {
        if (min) { *min = spinBox->minimum(); }
        if (max) { *max = spinBox->maximum(); }
    } else if (QDoubleSpinBox * spinBox = dynamic_cast<QDoubleSpinBox *>(controlled)) {
        if (min) { *min = spinBox->minimum(); }
        if (max) { *max = spinBox->maximum(); }
    } else if (QTimeEdit * spinBox = dynamic_cast<QTimeEdit *>(controlled)) {
        if (min) { *min = QTime(0, 0, 0, 0).msecsTo(spinBox->minimumTime()); }
        if (max) { *max = QTime(0, 0, 0, 0).msecsTo(spinBox->maximumTime()); }
    } else {
        QMetaObject::invokeMethod(controlled, "minimum", Q_RETURN_ARG(qreal, *min));
        QMetaObject::invokeMethod(controlled, "maximum", Q_RETURN_ARG(qreal, *max));
    }
}

qreal DragControllerPrivate::calcValue(const QPoint & offset, qreal min, qreal max)
{
    switch (type) {
        case DragController::Linear: return linearValue(offset);
        case DragController::Circular: return circularValue(offset, min, max);
        case DragController::CircularInf: return circularInfValue(offset, false);
        case DragController::CircularInfCw: return circularInfValue(offset, true);
    }
    return 0.0;
}

void DragControllerPrivate::setValue(QObject * controlled, const QPoint & offset)
{
    qreal min = 0.0;
    qreal max = 0.0;
    getRange(sender()->parent(), &min, &max);

    qreal value = calcValue(offset, min, max);

    if (QSpinBox * spinBox = dynamic_cast<QSpinBox *>(controlled)) {
        spinBox->setValue(qRound(value));
    } else if (QDoubleSpinBox * spinBox = dynamic_cast<QDoubleSpinBox *>(controlled)) {
        spinBox->setValue(value);
    } else if (QTimeEdit * spinBox = dynamic_cast<QTimeEdit *>(controlled)) {
        spinBox->setTime(QTime(0, 0, 0, 0).addMSecs(qRound(value) * 1000));
    } else {
        QMetaObject::invokeMethod(controlled, "setValue", Q_ARG(qreal, value));
    }
}

qreal DragControllerPrivate::gluedAngle(qreal currentAngle, const QList<qreal> & anchorAngles)
{
    qreal threshold = 0.2;
    foreach (qreal anchorAngle, anchorAngles)
    {
        if (currentAngle > anchorAngle - threshold &&
                currentAngle < anchorAngle + threshold) {
            return anchorAngle;
        }
    }
    return currentAngle;
}

void DragControllerPrivate::onPressed()
{
    QObject * controlled = sender()->parent();

    if (QSpinBox * spinBox = dynamic_cast<QSpinBox *>(controlled)) {
        lastPressedValue = spinBox->value();
    } else if (QDoubleSpinBox * spinBox = dynamic_cast<QDoubleSpinBox *>(controlled)) {
        lastPressedValue = spinBox->value();
    } else if (QTimeEdit * spinBox = dynamic_cast<QTimeEdit *>(controlled)) {
        lastPressedValue = QTime(0, 0, 0, 0).msecsTo(spinBox->time()) / 1000.0;
    } else {
        QMetaObject::invokeMethod(controlled, "value", Q_RETURN_ARG(qreal, lastPressedValue));
    }
}

void DragControllerPrivate::onReleased()
{
    lastAngle = -1.0;
    totalAngle = 0.0;
    if (QLineEdit * lineEdit = sender()->parent()->findChild<QLineEdit *>()) {
        lineEdit->end(false);
    }
}

void DragControllerPrivate::onDragged(const QPoint & offset)
{
    setValue(sender()->parent(), offset);
}

void DragControllerPrivate::showLinearOverlay()
{
    MouseEventNotifier * mouseEventNotifier = static_cast<MouseEventNotifier *>(sender());

    LinearOverlay * overlay = new LinearOverlay(static_cast<QWidget *>(QApplication::activeWindow()));
    overlay->setGeometry(overlayGeometry(static_cast<QWidget *>(mouseEventNotifier->parent())));

    connect(mouseEventNotifier, &MouseEventNotifier::dragged, overlay, &LinearOverlay::setOffset);
    connect(mouseEventNotifier, &MouseEventNotifier::dragged, overlay, static_cast<void (LinearOverlay::*)()>(&LinearOverlay::update));
    connect(mouseEventNotifier, &MouseEventNotifier::released, overlay, &LinearOverlay::close);

    overlay->show();
}

void DragControllerPrivate::showCircularOverlay()
{
    MouseEventNotifier * mouseEventNotifier = static_cast<MouseEventNotifier *>(sender());

    CircularOverlay * overlay = new CircularOverlay(static_cast<QWidget *>(QApplication::activeWindow()));
    overlay->setGeometry(overlayGeometry(static_cast<QWidget *>(mouseEventNotifier->parent())));

    if (type == DragController::Circular)
    {
        qreal adjustedMin = qMin(minPos, maxPos);
        qreal adjustedMax = qMax(minPos, maxPos);
        NORMALIZE(adjustedMin)
        NORMALIZE(adjustedMax)
        overlay->setBounds(adjustedMin, adjustedMax);
    }

    connect(mouseEventNotifier, &MouseEventNotifier::dragged, overlay, &CircularOverlay::setOffset);
    connect(mouseEventNotifier, &MouseEventNotifier::dragged, overlay, static_cast<void (CircularOverlay::*)()>(&CircularOverlay::update));
    connect(mouseEventNotifier, &MouseEventNotifier::released, overlay, &LinearOverlay::close);

    overlay->show();
}

}

#undef RADIUS_THRESHOLD
#undef RAD
#undef DEG
#undef NORMALIZE
#undef BOUND
#undef NORMALIZED
