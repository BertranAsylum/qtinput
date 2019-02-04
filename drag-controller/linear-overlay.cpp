#include "linear-overlay.h"

#include <QPainter>
#include <QPaintEvent>

namespace qtinput {

LinearOverlay::LinearOverlay(QWidget * parent) :
    Overlay(parent)
{}

void LinearOverlay::paintEvent(QPaintEvent * event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::transparent);
    painter.setBrush(QBrush(Qt::gray));
    painter.translate(width()/2., height()/2.);


    qreal stretchFactor = qreal(m_offset.y()) / qreal(height());
    QPolygon polygon = QPolygon() << QPoint(-20, 0) << QPoint(20, 0) << QPoint(0, height()/10);

    painter.save();

    painter.translate(0., height()/10.0 + 6. + stretchFactor);
    painter.drawPolygon(polygon);
    painter.translate(0., height()/8.0 + stretchFactor);
    painter.drawPolygon(polygon);

    painter.restore();

    painter.scale(1., -1.);

    painter.translate(0., height()/10.0 + 6. - stretchFactor);
    painter.drawPolygon(polygon);
    painter.translate(0., height()/8.0 - stretchFactor);
    painter.drawPolygon(polygon);

    event->accept();
}

}
