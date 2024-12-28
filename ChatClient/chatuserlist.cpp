#include "chatuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>

ChatUserList::ChatUserList(QWidget *parent)
    :QListWidget(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    // 检查鼠标进入或离开
    if (watched == viewport()) {
        if (event->type() == QEvent::Enter) {
            // 进入viewport显示滚动条
            setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        }else if(event->type() == QEvent::Leave){
            // 离开隐藏
            setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        }
    }

    // if (watched == this->viewport() && event->type() == QEvent::Wheel) {
    //     QWheelEvent* wheelEvent = static_cast<QWheelEvent*>(event);

    //     int degree = wheelEvent->angleDelta().y() / 8;
    //     int step = degree / 15;
    //     auto * scrollBar = verticalScrollBar();
    //     scrollBar->setValue(scrollBar->value() - step); // 调整滚动条的位置，从而实现滚动内容的上下移动
    //     int maxVal = scrollBar->maximum();
    //     int curVal = scrollBar->value();
    //     int pageSize = 10;

    //     if (maxVal - curVal <= 0) {
    //         // 滚动到底部，加载新的联系人
    //         qDebug()<<"load more chat user";
    //         //发送信号通知聊天界面加载更多聊天内容
    //         emit sig_loading_chat_user();
    //     }

    //     return true;
    // }

    // 逐像素滚动
    if (watched == this->viewport() && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent*>(event);

        // 优先使用 pixelDelta() 实现逐像素滚动
        if (!wheelEvent->pixelDelta().isNull()) {
            // 获取滚轮的垂直像素变化量
            int pixelDeltaY = wheelEvent->pixelDelta().y();
            verticalScrollBar()->setValue(verticalScrollBar()->value() - pixelDeltaY);
        } else {
            // pixelDelta() 不可用时，回退到逐角度滚动
            int numDegrees = wheelEvent->angleDelta().y() / 8;
            int pixelDeltaY = (numDegrees / 15.0);
            verticalScrollBar()->setValue(verticalScrollBar()->value() - pixelDeltaY);
        }

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        if (scrollBar->maximum() - scrollBar->value() <= 0) {
            // qDebug() << "load more chat user";
            emit sig_loading_chat_user();
        }

        return true;
    }


    return QListWidget::eventFilter(watched, event);
}
