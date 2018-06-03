#ifndef MOUSEEVENTNOTIFIER_H
#define MOUSEEVENTNOTIFIER_H

#include <QObject>
#include <QPoint>

namespace qtinput {

class MouseEventNotifier : public QObject {
    Q_OBJECT
    QPoint m_lastPressedPos;
public:
    explicit MouseEventNotifier(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void pressed();
    void released();
    void dragged(const QPoint &offset);
};

}

#endif //MOUSEEVENTNOTIFIER_H
