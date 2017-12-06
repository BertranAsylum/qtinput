#include "drag-event-notifier.h"

#include <QDebug>
#include <QMouseEvent>

using namespace InputTools;

DragEventNotifier::DragEventNotifier(QObject *parent)
    : QObject(parent) {}

bool DragEventNotifier::eventFilter(QObject *watched, QEvent *event) {
    if(event->type() == QEvent::MouseButtonPress) {
        mLastPressedPos = static_cast<QMouseEvent*>(event)->pos();
        emit pressed();
    } else if(event->type() == QEvent::MouseMove) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if((mouseEvent->buttons() & Qt::LeftButton) != 0) {
            emit dragged(mouseEvent->pos() - mLastPressedPos);
        }
    }
    return QObject::eventFilter(watched, event);
}
