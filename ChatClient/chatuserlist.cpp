#include "chatuserlist.h"
#include <QEvent>
#include <QWheelEvent>
#include <QDebug>
#include <QScrollBar>
#include <QMovie>
#include <QScroller>

ChatUserList::ChatUserList(QWidget *parent)
    :QListView(parent)
{
    // 创建模型和代理
    model_ = new ChatUserListModel(this);
    delegate_ = new ChatUserDelegate(this);
    setModel(model_);
    setItemDelegate(delegate_);
    // 连接模型加载数据信号
    // connect(model_, &ChatUserListModel::sig_loading_started, this, &ChatUserList::slot_showLoadingAnimation);
    // connect(model_, &ChatUserListModel::sig_loading_finished, this, &ChatUserList::slot_hideLoadingAnimation);
    // 设置鼠标悬停提示
    // setMouseTracking(true);  // 启用鼠标跟踪，允许触发 hover 事件
    // 初始化滚动条
    initCustomScrollBar();

    // 事件过滤器，用于检测滚动事件
    viewport()->installEventFilter(this);
}

bool ChatUserList::eventFilter(QObject *watched, QEvent *event)
{
    //检查鼠标进入或离开
    if ((watched == viewport() || watched == scrollBar_) && event->type() == QEvent::Enter && scrollBar_->maximum() > 0)
    {
        // 如果鼠标进入 viewport 或 scrollBar_，显示 scrollBar_
        scrollBar_->setVisible(true);
        return true; // 事件已处理
    }
    // 只有 viewport 和 scrollBar_ 同时发生 Leave 事件时隐藏 scrollBar_
    if ((watched == viewport() || watched == scrollBar_) && event->type() == QEvent::Leave) {
        // 获取鼠标是否离开了 viewport 和 scrollBar_ 的区域
        QPoint globalMousePos = QCursor::pos();
        QPoint viewportPos = viewport()->mapToGlobal(QPoint(0, 0));
        QPoint scrollBarPos = scrollBar_->mapToGlobal(QPoint(0, 0));

        QRect viewportRect(viewportPos, viewport()->size());
        QRect scrollBarRect(scrollBarPos, scrollBar_->size());

        if (!viewportRect.contains(globalMousePos) && !scrollBarRect.contains(globalMousePos))
        {
            scrollBar_->setVisible(false); // 如果鼠标不在两者范围内，则隐藏 scrollBar_
        }
        return true; // 事件已处理
    }

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
            int pixelDeltaY = (numDegrees / 15.0) * 30;
            verticalScrollBar()->setValue(verticalScrollBar()->value() - pixelDeltaY);
        }

        // 检查是否滚动到底部
        QScrollBar *scrollBar = this->verticalScrollBar();
        if (scrollBar->maximum() - scrollBar->value() <= 0) {
            emit sig_loading_chat_user();
        }
        return true;
    }

    return QListView::eventFilter(watched, event);
}

void ChatUserList::resizeEvent(QResizeEvent *event)
{
    // 要想让自定义滚动条能正常工作，就不能在if (scrollBar_ && scrollBar_->isVisible())条件中写
    scrollBar_->setGeometry(width() - 8, 1, 8, height());
    slot_range_change(verticalScrollBar()->minimum(), verticalScrollBar()->maximum());
    return QListView::resizeEvent(event);
}

void ChatUserList::initCustomScrollBar()
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // 创建QScrollBar
    scrollBar_ = new QScrollBar(Qt::Vertical, this);
    scrollBar_->installEventFilter(this);
    /*
     * 循环问题如何避免?
     * 滚动条的 setValue() 方法内部有优化逻辑，当新值等于当前值时不会触发 valueChanged 信号。
     * 可以放心地使用双向连接而不会担心出现无限循环的问题。
     * 例如，scrollBar_->setValue() 被调用后，如果值未变化，则不会重复发出信号，避免了无限循环。
     * 在双向连接中，通过 valueChanged 信号和 setValue() 的配合，两个滚动条的状态可以实时保持一致。
     */
    connect(verticalScrollBar(), &QScrollBar::valueChanged, scrollBar_, &QScrollBar::setValue);
    connect(scrollBar_, &QScrollBar::valueChanged, verticalScrollBar(), &QScrollBar::setValue);
    connect(verticalScrollBar(), &QScrollBar::rangeChanged, this, &ChatUserList::slot_range_change);
    scrollBar_->setVisible(false);
}

void ChatUserList::slot_showLoadingAnimation()
{

}

void ChatUserList::slot_hideLoadingAnimation()
{

}

void ChatUserList::slot_range_change(int min, int max)
{
    scrollBar_->setRange(min, max);

    int step = verticalScrollBar()->pageStep();
    if (max > 0) {
        scrollBar_->setPageStep(step);  // 确保 pageStep 不超过范围
        scrollBar_->setVisible(true);
    } else {
        scrollBar_->setPageStep(0);  // 如果没有范围，不设置 pageStep
    }
}
