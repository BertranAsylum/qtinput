#include "circular-overlay.h"

#include <QPainter>
#include <QPaintEvent>

#define NORMALIZE(rad) rad = fmod(rad, 2.0*M_PI); \
                       if (rad < 0.0) { rad += 2.0*M_PI; }

#define BOUND(min,rad,max) if(rad < min || rad > max) { \
                               qreal minDist = qAbs(rad-min); if(minDist > M_PI) { minDist = 2.0*M_PI - minDist; } \
                               qreal maxDist = qAbs(rad-max); if(maxDist > M_PI) { maxDist = 2.0*M_PI - maxDist; } \
                               if(minDist < maxDist) { rad = min; } else { rad = max; } \
                           }

namespace qtinput {

CircularOverlay::CircularOverlay(QWidget * parent) :
    Overlay(parent)
{
    m_offset.ry() -= height()/2;
}

void CircularOverlay::setBounds(qreal min, qreal max)
{
    if (!qFuzzyCompare(min, max)) {
        m_min = min;
        m_max = max;
    }
}

void CircularOverlay::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPen pen(Qt::gray);
    pen.setWidthF(1.5);
    painter.setPen(pen);
    painter.drawEllipse(QRect(2, 2, width()-4, height()-4));

    qreal angle = atan2(-m_offset.y(), m_offset.x());
    NORMALIZE(angle)

    pen.setWidthF(5);
    painter.setPen(pen);
    painter.drawArc(QRect(7, 7, width()-14, height()-14), qRound(-110.0+ angle*180.0/M_PI)*16, 40*16);
    painter.drawArc(QRect(7, 7, width()-14, height()-14), qRound(70.0+ angle*180.0/M_PI)*16, 40*16);

    painter.setPen(QColor(Qt::transparent));
    painter.setBrush(QBrush(Qt::gray));
    painter.translate(width()/2, height()/2);

    BOUND(m_min, angle, m_max)
    painter.rotate(-angle*180./M_PI);
    painter.drawPolygon(QPolygon() << QPoint(width()/2 - 4, 0)
                        << QPoint(width()/2 - 12, 6)
                        << QPoint(width()/2 - 12, -6));
    event->accept();
}

}

#undef NORMALIZE
#undef BOUND
