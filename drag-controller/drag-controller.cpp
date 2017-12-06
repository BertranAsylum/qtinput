#include "drag-controller.h"

#include <QDebug>
#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QTimeEdit>
#include <math.h>

#include "drag-event-notifier.h"

using namespace InputTools;

void DragController::attach(QWidget *controlled, const Mode &mode, double sensitivity) {
    new DragController(controlled, mode, sensitivity);
}

DragController::DragController(QWidget *controlled, const Mode &mode, double sensitivity)
    : QObject(static_cast<QObject*>(controlled)),
      mMode(mode),
      mSensitivity(sensitivity),
      mLastPressedValue(0.) {
    installDragEventNotifier(controlled);
}

void DragController::installDragEventNotifier(QWidget *controlled) {
    if(dynamic_cast<QAbstractSpinBox*>(controlled)) {
        if(QLineEdit *lineEdit = controlled->findChild<QLineEdit*>()) {
            DragEventNotifier *dragEventNotifier = new DragEventNotifier(controlled);
            connect(dragEventNotifier, SIGNAL(pressed()), this, SLOT(onPressed()));
            connect(dragEventNotifier, SIGNAL(dragged(QPoint)), this, SLOT(onDragged(QPoint)));
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
    return (angle/(2*M_PI)) * (max - min) + min;
}

double DragController::circularSymValue(const QPoint &offset, double min, double max) {
    double angle = atan2(offset.x(), -offset.y());
    return ((angle + M_PI)/(2*M_PI)) * (max - min) + min;
}

void DragController::onPressed() {
    QObject *controlled = sender()->parent();
    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        mLastPressedValue = spinBox->value();
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        mLastPressedValue = spinBox->value();
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        mLastPressedValue = spinBox->time().msecsSinceStartOfDay();
    }
}

void DragController::onDragged(const QPoint &offset) {
    double min = 0.,
           max = 0.;
    QObject *controlled = sender()->parent();
    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        min = spinBox->minimum();
        max = spinBox->maximum();
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        min = spinBox->minimum();
        max = spinBox->maximum();
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        min = spinBox->minimumTime().msecsSinceStartOfDay();
        max = spinBox->maximumTime().msecsSinceStartOfDay();
    }

    double value = 0.;
    switch(mMode) {
        case LINEAR: value = linearValue(offset); break;
        case CIRCULAR: value = circularValue(offset, min, max); break;
        case CIRCULAR_SYM: value = circularSymValue(offset, min, max); break;
    }

    if(QSpinBox *spinBox = dynamic_cast<QSpinBox*>(controlled)) {
        spinBox->setValue(qRound(value));
    } else if(QDoubleSpinBox *spinBox = dynamic_cast<QDoubleSpinBox*>(controlled)) {
        spinBox->setValue(value);
    } else if(QTimeEdit *spinBox = dynamic_cast<QTimeEdit*>(controlled)) {
        spinBox->setTime(QTime::fromMSecsSinceStartOfDay(value));
    }
}

