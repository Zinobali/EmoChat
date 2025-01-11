#include "chatuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>
#include <QMovie>
#include <QScroller>
#include <QTimer>
#include "usermgr.h"

ChatUserList::ChatUserList(QWidget *parent)
    : QListWidget(parent)
{
    // 隐藏滚动条
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 事件过滤器，用于检测滚动事件
    viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查事件是否是鼠标悬浮进入或离开
    if (watched == this->viewport())
    {
        if (event->type() == QEvent::Enter)
        {
            // 鼠标悬浮，显示滚动条
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }
        else if (event->type() == QEvent::Leave)
        {
            // 鼠标离开，隐藏滚动条
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // 逐像素滚动
    if (watched == this->viewport() && event->type() == QEvent::Wheel)
    {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

        // 优先使用 pixelDelta() 实现逐像素滚动
        if (!wheelEvent->pixelDelta().isNull())
        {
            // 获取滚轮的垂直像素变化量
            int pixelDeltaY = wheelEvent->pixelDelta().y();
            verticalScrollBar()->setValue(verticalScrollBar()->value() - pixelDeltaY);
        }
        else
        {
            // pixelDelta() 不可用时，回退到逐角度滚动
            int numDegrees = wheelEvent->angleDelta().y() / 8;
            int pixelDeltaY = (numDegrees / 15.0) * 30;
            verticalScrollBar()->setValue(verticalScrollBar()->value() - pixelDeltaY);
        }

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        if (scrollBar->maximum() - scrollBar->value() <= 0)
        {
            auto load_finish = UserMgr::GetInstance()->IsChatListLoadFinish();
            if (load_finish)
            {
                return true;
            }

            if (_load_pending)
            {
                return true;
            }

            _load_pending = true;
            QTimer::singleShot(100, [this]()
                               { _load_pending = false; });
            emit sig_loading_chat_user();
        }
        return true;
    }

    return QListWidget::eventFilter(watched, event);
}
