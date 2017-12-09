#include "linear-overlay.h"

#include <QPaintEvent>
#include <QPainter>

using namespace QtInputTools;

LinearOverlay::LinearOverlay(QWidget *parent)
    : Overlay(parent) {}

void LinearOverlay::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::transparent);
    painter.setBrush(QBrush(Qt::gray));
    painter.translate(width()/2., height()/2.);
    int size = width()/5.;
    QPolygon polygon = QPolygon() << QPoint(-size, 0) << QPoint(size, 0) << QPoint(0, size/2);
    painter.save();
    painter.translate(0., width()/5.);
    painter.drawPolygon(polygon);
    painter.translate(0., width()/5.);
    painter.drawPolygon(polygon);
    painter.restore();
    painter.scale(1., -1.);
    painter.translate(0., width()/5.);
    painter.drawPolygon(polygon);
    painter.translate(0., width()/5.);
    painter.drawPolygon(polygon);
    event->accept();
}
