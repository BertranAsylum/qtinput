#ifndef QTINPUT_CIRCULAROVERLAY_H
#define QTINPUT_CIRCULAROVERLAY_H

#include <math.h>
#include "overlay.h"

namespace qtinput {

class CircularOverlay : public Overlay {
    Q_OBJECT

public:
    explicit CircularOverlay(QWidget * parent = nullptr);
    void setBounds(qreal min, qreal max);

public slots:
    void setOffset(const QPoint & offset) { m_offset = offset; }

protected:
    void paintEvent(QPaintEvent * event);

private:
    QPoint m_offset;
    qreal m_min = 0.0;
    qreal m_max = 2.0*M_PI;
};

}

#endif // QTINPUT_CIRCULAROVERLAY_H
