#include "friendlistdelegate.h"

FriendListDelegate::FriendListDelegate(QWidget *parent)
    : QStyledItemDelegate{parent}
{}

QSize FriendListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(option.rect.width(), 50);
}

void FriendListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    std::shared_ptr<User> user = index.data(Qt::UserRole).value<std::shared_ptr<User>>();
    //用户名
    painter->drawText(option.rect.x() + 60, option.rect.y() + 30, user->username());
    //头像
    QPixmap avatar;
    if (user->avatar().isEmpty())
    {
        avatar = QPixmap(":/icon/img/ultraman.png");
    }
    else
    {
        avatar.loadFromData(user->avatar(), "JPG");
    }
    painter->drawPixmap(option.rect.x() + 5, option.rect.y() + 5, 40, 40, avatar);
    //父对象的painter
    QStyledItemDelegate::paint(painter, option, index);
}
