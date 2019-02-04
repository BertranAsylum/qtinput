#ifndef QTINPUT_OVERLAY_H
#define QTINPUT_OVERLAY_H

#include <QWidget>

class QPropertyAnimation;

namespace qtinput {

class Overlay : public QWidget {
    Q_OBJECT

public:
    explicit Overlay(QWidget * parent = nullptr);

protected:
    void showEvent(QShowEvent * event);
    void closeEvent(QCloseEvent * event);

private:
    QPropertyAnimation * m_animation;
};

}

#endif // QTINPUT_OVERLAY_H
