#include "circular-overlay.h"

#include <QPaintEvent>
#include <QPainter>
#include <math.h>

#define ADJUST(rad) while(rad < 0.0) { rad += 2.0*M_PI; } \
                    while(rad > 2.0*M_PI) { rad -= 2.0*M_PI; }
#define BOUND(min,rad,max) if(rad < min || rad > max) { \
                               double minDist = qAbs(rad-min); if(minDist > M_PI) { minDist = 2.0*M_PI - minDist; } \
                               double maxDist = qAbs(rad-max); if(maxDist > M_PI) { maxDist = 2.0*M_PI - maxDist; } \
                               if(minDist < maxDist) { rad = min; } else { rad = max; } \
                           }

using namespace qtinput;

CircularOverlay::CircularOverlay(QWidget *parent)
    : Overlay(parent) {
    m_offset.ry() -= height()/2;
}

void CircularOverlay::setBounds(double min, double max) {
    m_min = min;
    m_max = max;
}

void CircularOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen(Qt::gray);
    pen.setWidthF(1.5);
    painter.setPen(pen);
    painter.drawEllipse(QRect(2, 2, width()-4, height()-4));

    double angle = atan2(-m_offset.y(), m_offset.x());
    ADJUST(angle)
    pen.setWidthF(5);
    painter.setPen(pen);
    painter.drawArc(QRect(7, 7, width()-14, height()-14), (-110.+ angle*180./M_PI)*16., 40.*16.);
    painter.drawArc(QRect(7, 7, width()-14, height()-14), (70.+ angle*180./M_PI)*16., 40.*16.);

    painter.setPen(QColor(Qt::transparent));
    painter.setBrush(QBrush(Qt::gray));
    painter.translate(width()/2, height()/2);
    BOUND(m_min, angle, m_max)
    painter.rotate(-angle*180./M_PI);
    painter.drawPolygon( QPolygon() << QPoint(width()/2 - 4, 0)
                                    << QPoint(width()/2 - 12, 6)
                                    << QPoint(width()/2 - 12, -6));
    event->accept();
}
