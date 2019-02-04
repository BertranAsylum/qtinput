#include "overlay.h"

#include <QShowEvent>
#include <QPropertyAnimation>

namespace qtinput {

Overlay::Overlay(QWidget * parent) :
    QWidget(parent),
    m_animation(new QPropertyAnimation(this, "geometry"))
{
    setAttribute(Qt::WA_NoSystemBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);

    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::OutQuad);
}

void Overlay::showEvent(QShowEvent * event)
{
    QRect startGeometry = geometry();
    startGeometry.setX(startGeometry.x() + width()/2);
    startGeometry.setY(startGeometry.y() + height()/2);
    startGeometry.setSize(QSize());

    m_animation->setStartValue(startGeometry);
    m_animation->setEndValue(geometry());
    m_animation->start();

    QWidget::showEvent(event);
}

void Overlay::closeEvent(QCloseEvent * event)
{
    connect(m_animation, &QPropertyAnimation::finished, this, &Overlay::deleteLater);

    m_animation->setDirection(QAbstractAnimation::Backward);
    m_animation->start();

    event->ignore();
}

}
