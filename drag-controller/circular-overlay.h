#ifndef CIRCULAROVERLAY_H
#define CIRCULAROVERLAY_H

#include "overlay.h"

namespace qtinput {

class CircularOverlay : public Overlay {
    Q_OBJECT
    QPoint m_offset;
    double m_min = 0.0;
    double m_max = 2.0*M_PI;

public:
    explicit CircularOverlay(QWidget *parent = 0);
    void setBounds(double min, double max);

public slots:
    void setOffset(const QPoint &offset) { m_offset = offset; }

protected:
    void paintEvent(QPaintEvent *event);
};

}

#endif // CIRCULAROVERLAY_H
