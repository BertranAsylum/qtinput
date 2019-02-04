#include "mouse-event-notifier.h"

#include <QDebug>
#include <QMouseEvent>

namespace qtinput {

MouseEventNotifier::MouseEventNotifier(QObject * parent) :
    QObject(parent)
{}

bool MouseEventNotifier::eventFilter(QObject * watched, QEvent * event)
{
    Q_UNUSED(watched)

    if (event->type() == QEvent::MouseButtonPress)
    {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        if ((mouseEvent->buttons() & Qt::LeftButton) != 0) {
            m_lastPressedPos = mouseEvent->pos();
            emit pressed();
            return true;
        }
    }
    else if (event->type() == QEvent::MouseButtonRelease)
    {
        emit released();
        return true;
    }
    else if (event->type() == QEvent::MouseMove)
    {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        if ((mouseEvent->buttons() & Qt::LeftButton) != 0) {
            emit dragged(mouseEvent->pos() - m_lastPressedPos);
            return true;
        }
    }
    return false;
}

}
