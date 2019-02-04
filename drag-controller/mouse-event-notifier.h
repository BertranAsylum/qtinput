#ifndef QTINPUT_MOUSEEVENTNOTIFIER_H
#define QTINPUT_MOUSEEVENTNOTIFIER_H

#include <QObject>
#include <QPoint>

namespace qtinput {

class MouseEventNotifier : public QObject {
    Q_OBJECT
public:
    explicit MouseEventNotifier(QObject * parent = nullptr);

protected:
    bool eventFilter(QObject * watched, QEvent * event);

signals:
    void pressed();
    void released();
    void dragged(const QPoint & offset);

private:
    QPoint m_lastPressedPos;
};

}

#endif //QTINPUT_MOUSEEVENTNOTIFIER_H
