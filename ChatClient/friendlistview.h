#ifndef FRIENDLISTVIEW_H
#define FRIENDLISTVIEW_H

#include <QWidget>
#include <QListView>
#include <QStringListModel>
#include <QStandardItemModel>
#include "friendlistdelegate.h"
#include "user.h"
#include <memory>


class FriendListView : public QListView
{
    Q_OBJECT
public:
    explicit FriendListView(QWidget *parent = nullptr);
    void setFriends(const QVector<User> &friends);

signals:

private:
    void updateModel();

private:
    QStandardItemModel *m_model;
    FriendListDelegate *m_delegate;
    QVector<std::shared_ptr<User>> m_friends;

};

#endif // FRIENDLISTVIEW_H
