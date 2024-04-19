#ifndef CHATLISTDELEGATE_H
#define CHATLISTDELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <memory>
#include "user.h"
#include <QPainter>

class ChatListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit ChatListDelegate(QObject *parent = nullptr);

    std::shared_ptr<User> currentUser() const;
    void setCurrentUser(const std::shared_ptr<User> &newCurrentUser);
    std::shared_ptr<User> friendUser() const;
    void setFriendUser(const std::shared_ptr<User> &newFriendUser);

signals:

    // QAbstractItemDelegate interface
public:
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    std::shared_ptr<User> m_currentUser;
    std::shared_ptr<User> m_friendUser;

};

#endif // CHATLISTDELEGATE_H
