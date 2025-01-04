#include "applyfriendlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QScrollBar>
#include <QDebug>

ApplyFriendList::ApplyFriendList(QWidget *parent)
    : QListWidget(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);
}

bool ApplyFriendList::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == viewport())
    {
        // 检查鼠标进入和离开事件
        if (event->type() == QEvent::Enter)
        {
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }

        // 检查鼠标按下事件
        if (event->type() == QEvent::MouseButtonPress)
        {
            emit sig_show_search(false);
        }

        // 检查滚轮事件
        if (event->type() == QEvent::Wheel)
        {
            QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);
            int numDegrees = wheelEvent->angleDelta().y() / 8;
            int numSteps = numDegrees / 15; // 计算滚动步数

            // 设置滚动幅度
            verticalScrollBar()->setValue(verticalScrollBar()->value() - numSteps);
            return true; // 停止事件传递
        }
    }

    return QListWidget::eventFilter(watched, event);
}
