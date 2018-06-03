#include "overlay.h"

#include <QPropertyAnimation>
#include <QShowEvent>

using namespace qtinput;

Overlay::Overlay(QWidget *parent)
    : QWidget(parent) {
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    mAnimation = new QPropertyAnimation(this, "geometry");
    mAnimation->setDuration(200);
    mAnimation->setEasingCurve(QEasingCurve::OutQuad);
}

void Overlay::showEvent(QShowEvent *event) {
    QRect startGeometry = geometry();
    startGeometry.setX(startGeometry.x() + width()/2);
    startGeometry.setY(startGeometry.y() + height()/2);
    startGeometry.setSize(QSize());
    mAnimation->setStartValue(startGeometry);
    mAnimation->setEndValue(geometry());
    mAnimation->start();
    QWidget::showEvent(event);
}

void Overlay::closeEvent(QCloseEvent *event) {
    mAnimation->setDirection(QAbstractAnimation::Backward);
    mAnimation->start();
    connect(mAnimation, SIGNAL(finished()), this, SLOT(deleteLater()));
    event->ignore();
}
