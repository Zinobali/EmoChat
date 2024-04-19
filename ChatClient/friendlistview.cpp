#include "friendlistview.h"

FriendListView::FriendListView(QWidget *parent)
    : QListView(parent)
{
    m_model = new QStandardItemModel(this);
    this->setModel(m_model);
    m_delegate = new FriendListDelegate(this);
    this->setItemDelegate(m_delegate);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->setSelectionMode(QAbstractItemView::SingleSelection);

    for (int id = 0; id < 5; id++)
    {
        User user;
        user.setId(id);
        user.setUsername(QString("user%1").arg(id));
        std::shared_ptr<User> user_ptr = std::make_shared<User>(user);
        m_friends.append(user_ptr);
    }
    updateModel();

}

void FriendListView::setFriends(const QVector<User> &friends)
{
    for (auto user : friends)
    {
        m_friends.append(std::make_shared<User>(user));
    }
    updateModel();
}

void FriendListView::updateModel()
{
    m_model->clear();
    for(auto user : m_friends )
    {
        QStandardItem *item = new QStandardItem();
        //直接把user设置进去
        QVariant userData;
        userData.setValue(user);
        item->setData(userData, Qt::UserRole);
        m_model->appendRow(item);
    }
}
