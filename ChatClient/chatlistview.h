#ifndef CHATLISTVIEW_H
#define CHATLISTVIEW_H

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include "chatlistdelegate.h"
#include "user.h"
#include <memory>

class ChatListView : public QListView
{
    Q_OBJECT
public:
    explicit ChatListView(QWidget *parent = nullptr);

    std::shared_ptr<User> currentUser() const;
    void setCurrentUser(const std::shared_ptr<User> &newCurrentUser);
    std::shared_ptr<User> friendUser() const;
    void setFriendUser(const std::shared_ptr<User> &newFriendUser);

signals:

private:
    // QStandardItemModel *m_model;
    ChatListDelegate* m_delegate;
    std::shared_ptr<User> m_currentUser;
    std::shared_ptr<User> m_friendUser;
    
};

#endif // CHATLISTVIEW_H
