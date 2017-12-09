#include "circular-overlay.h"

#include <QPaintEvent>
#include <QPainter>
#include <math.h>

using namespace QtInputTools;

CircularOverlay::CircularOverlay(QWidget *parent)
    : Overlay(parent) {}

void CircularOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(QColor("gray"));
    painter.drawEllipse(QRect(3, 3, width()-6, height()-6));

    double angle = atan2(mOffset.x(), -mOffset.y());
    painter.drawArc(QRect(6, 6, width()-12, height()-12), (-20.- angle*180./M_PI)*16., 40.*16.);
    painter.drawArc(QRect(6, 6, width()-12, height()-12), (160.- angle*180./M_PI)*16., 40.*16.);
    event->accept();
}

