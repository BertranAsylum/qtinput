#ifndef LINEAROVERLAY_H
#define LINEAROVERLAY_H

#include "overlay.h"

namespace qtinput {

class LinearOverlay : public Overlay {
    Q_OBJECT
public:
    explicit LinearOverlay(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *event);
};

}

#endif // LINEAROVERLAY_H
