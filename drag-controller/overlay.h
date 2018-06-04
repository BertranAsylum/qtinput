#ifndef OVERLAY_H
#define OVERLAY_H

#include <QWidget>

class QPropertyAnimation;

namespace qtinput {

class Overlay : public QWidget {
    Q_OBJECT
    QPropertyAnimation *m_animation;

public:
    explicit Overlay(QWidget *parent = 0);

protected:
    void showEvent(QShowEvent *event);
    void closeEvent(QCloseEvent *event);
};

}

#endif // OVERLAY_H
