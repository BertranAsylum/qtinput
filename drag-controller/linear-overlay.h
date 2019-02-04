#ifndef QTINPUT_LINEAROVERLAY_H
#define QTINPUT_LINEAROVERLAY_H

#include "overlay.h"

namespace qtinput {

class LinearOverlay : public Overlay {
    Q_OBJECT

public:
    explicit LinearOverlay(QWidget * parent = nullptr);

public slots:
    void setOffset(const QPoint & offset) { m_offset = offset; }

protected:
    void paintEvent(QPaintEvent * event);

private:
    QPoint m_offset;
};

}

#endif // QTINPUT_LINEAROVERLAY_H
