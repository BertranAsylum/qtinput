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
    QPen pen(Qt::gray);
    pen.setWidthF(1.5);
    painter.setPen(pen);
    painter.drawEllipse(QRect(2, 2, width()-4, height()-4));

    double angle = atan2(mOffset.x(), -mOffset.y());
    pen.setWidthF(5);
    painter.setPen(pen);
    painter.drawArc(QRect(7, 7, width()-14, height()-14), (-20.- angle*180./M_PI)*16., 40.*16.);
    painter.drawArc(QRect(7, 7, width()-14, height()-14), (160.- angle*180./M_PI)*16., 40.*16.);
    event->accept();
}

