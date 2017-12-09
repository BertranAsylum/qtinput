#ifndef DRAGEVENTNOTIFIER_H
#define DRAGEVENTNOTIFIER_H

#include <QObject>
#include <QPoint>

namespace QtInputTools {

class DragEventNotifier : public QObject {
    Q_OBJECT
    QPoint mLastPressedPos;
public:
    explicit DragEventNotifier(QObject *parent = 0);

protected:
    bool eventFilter(QObject *watched, QEvent *event);

signals:
    void pressed();
    void released();
    void dragged(const QPoint &offset);
};

}

#endif // DRAGEVENTNOTIFIER_H
