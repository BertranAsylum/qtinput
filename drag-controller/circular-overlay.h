#ifndef CIRCULAROVERLAY_H
#define CIRCULAROVERLAY_H

#include "overlay.h"

namespace QtInputTools {

class CircularOverlay : public Overlay {
    Q_OBJECT
    QPoint mOffset;

public:
    explicit CircularOverlay(QWidget *parent = 0);

public slots:
    void setOffset(const QPoint &offset) { mOffset = offset; }

protected:
    void paintEvent(QPaintEvent *event);
};

}

#endif // CIRCULAROVERLAY_H
