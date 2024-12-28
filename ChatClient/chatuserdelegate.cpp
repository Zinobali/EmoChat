#include "chatuserdelegate.h"
#include "chatuserlistmodel.h"

#include <QFont>
#include <QPainter>

ChatUserDelegate::ChatUserDelegate(QObject *parent)
    :QStyledItemDelegate(parent)
{}

void ChatUserDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    auto name = index.data(ChatUserRoles::RoleName).toString();
    auto head = index.data(ChatUserRoles::RoleHead).value<QPixmap>();
    auto msg = index.data(ChatUserRoles::RoleMsg).toString();
    auto time = index.data(ChatUserRoles::RoleTime).toDateTime();

    painter->save();

    // 设置字体
    QFont nameFont("Microsoft YaHei", 12);
    QFont msgFont("Microsoft YaHei", 10);
    QFont timeFont("Microsoft YaHei", 9);

    // 提取绘制区域
    QRect itemRect = option.rect;

    if(option.state & QStyle::State_Selected){
         painter->fillRect(itemRect, QColor(211, 215, 212));  // 选中时背景色
    }else if (option.state & QStyle::State_MouseOver) {
        // 如果鼠标悬停项，设置不同的背景色
        painter->fillRect(itemRect, QColor(206, 207, 208));  // 悬停时背景色
    } else {
        // 默认背景色
        painter->fillRect(itemRect, QColor(247, 247, 247));  // 默认背景色
    }

    // 绘制头像
    int headSize = 50;
    QRect headRect(itemRect.left() + 5, itemRect.top() + 5, headSize, headSize);
    QPixmap scaledHead = head.scaled(headSize, headSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    painter->drawPixmap(headRect, scaledHead);  // 在指定位置绘制头像

    // 绘制用户名
    int nameHeight = 25; // 用户名高度
    QRect nameRect(headRect.right() + 10, headRect.top(), itemRect.width() - headRect.width() - 15, nameHeight);
    painter->setFont(nameFont);
    painter->setPen(Qt::black);
    painter->drawText(nameRect, Qt::AlignLeft | Qt::AlignVCenter, name);

    // 绘制时间
    int timeWidth = 60; // 时间区域宽度
    auto displayTime = parseTime(time);
    QRect timeRect(itemRect.right() - timeWidth - 10, headRect.top() + 5, timeWidth, 20);
    painter->setFont(timeFont);
    painter->setPen(Qt::gray);
    painter->drawText(timeRect, Qt::AlignRight | Qt::AlignVCenter, displayTime);

    // 绘制消息
    QRect msgRect(nameRect.left(), nameRect.bottom() + 5, timeRect.left() - nameRect.left() - 5, nameHeight);
    QFontMetrics msgMetrics(msgFont);
    QString elidedMsg = msgMetrics.elidedText(msg, Qt::ElideRight, msgRect.width());
    painter->setFont(msgFont);
    painter->setPen(Qt::darkGray);
    painter->drawText(msgRect, Qt::AlignLeft | Qt::AlignVCenter, elidedMsg);

    painter->restore();  // 恢复原来的绘制状态
}

QSize ChatUserDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);
    Q_UNUSED(index);
    return QSize(300, 70);
}

QString ChatUserDelegate::parseTime(const QDateTime &time) const
{
    QDateTime current = QDateTime::currentDateTime();
    QDate date = time.date();
    QDate currentDate = current.date();

    if (date == currentDate) {
        // 今天，显示时分
        return time.toString("hh:mm");
    } else if (date == currentDate.addDays(-1)) {
        // 昨天
        return "昨天";
    } else if (date > currentDate.addDays(-7)) {
        // 一周内，显示星期几
        QString weekDayNames[] = {"星期日", "星期一", "星期二", "星期三", "星期四", "星期五", "星期六"};
        return weekDayNames[date.dayOfWeek() - 1];
    } else {
        // 超过一周，显示日期
        return time.toString("yy/MM/dd");
    }
}
