#include "chatlistdelegate.h"

ChatListDelegate::ChatListDelegate(QObject *parent)
    : QStyledItemDelegate{parent}
{}

void ChatListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    int id = index.data(Qt::UserRole).toInt();
    qDebug() << "id:" << id;
    QString message = index.data(Qt::UserRole+1).toString();
    if(m_currentUser == nullptr)
    {
        qDebug() << "currentUser is nullptr";
        return;
    }
    if (id == m_currentUser->id())
    {
        //靠左显示内容
        // qDebug() << "左";
        painter->drawText(option.rect, Qt::AlignLeft, message);
    }
    else
    {
        //靠右显示内容
        // qDebug() << "右";
        painter->drawText(option.rect, Qt::AlignRight, message);
    }
    
    QStyledItemDelegate::paint(painter, option, index);
}

QSize ChatListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(100, 50);
}

std::shared_ptr<User> ChatListDelegate::currentUser() const
{
    return m_currentUser;
}

void ChatListDelegate::setCurrentUser(const std::shared_ptr<User> &newCurrentUser)
{
    m_currentUser = newCurrentUser;
}

std::shared_ptr<User> ChatListDelegate::friendUser() const
{
    return m_friendUser;
}

void ChatListDelegate::setFriendUser(const std::shared_ptr<User> &newFriendUser)
{
    m_friendUser = newFriendUser;
}
