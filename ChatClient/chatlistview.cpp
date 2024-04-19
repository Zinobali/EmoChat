#include "chatlistview.h"

ChatListView::ChatListView(QWidget *parent)
    : QListView{parent}
{
    m_delegate = new ChatListDelegate(this);
    setItemDelegate(m_delegate);
    //隐藏滚动条
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

std::shared_ptr<User> ChatListView::currentUser() const
{
    return m_currentUser;
}

void ChatListView::setCurrentUser(const std::shared_ptr<User> &newCurrentUser)
{
    m_currentUser = newCurrentUser;
    m_delegate->setCurrentUser(newCurrentUser);
    // qDebug()<<"ChatListView的currentUser" << m_currentUser->username();
}

std::shared_ptr<User> ChatListView::friendUser() const
{
    return m_friendUser;
}

void ChatListView::setFriendUser(const std::shared_ptr<User> &newFriendUser)
{
    m_friendUser = newFriendUser;
    m_delegate->setFriendUser(newFriendUser);
}
