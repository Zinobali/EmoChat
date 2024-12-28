#include "chatuserlistmodel.h"

#include <QPixmap>

ChatUserListModel::ChatUserListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

void ChatUserListModel::AddChatUser(const QString &name, const QString &headUrl, const QString &msg, const QDateTime &lastMsgTime)
{
    ChatUser u;
    u.name = name;
    u.head = headUrl;
    u.msg = msg;
    u.lastMsgTime = lastMsgTime;

    beginInsertRows(QModelIndex(), users_.size(), users_.size());
    users_.append(u);
    endInsertRows();
}

void ChatUserListModel::AddChatUsers(const QVector<ChatUser> &users)
{
    if (users.isEmpty()) {
        return;
    }

    emit sig_loading_started();
    beginInsertRows(QModelIndex(), users_.size(), users_.size() + users.size() - 1);
    users_ += users;
    endInsertRows();
    emit sig_loading_finished();
}

int ChatUserListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return users_.size();
}

QVariant ChatUserListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= users_.size()) {
        return QVariant();
    }

    const ChatUser& userData = users_[index.row()];
    if (role == ChatUserRoles::RoleName) {
        return userData.name; // 返回昵称
    }else if (role == ChatUserRoles::RoleHead) {
        return QPixmap(userData.head); // 返回头像
    }else if (role == ChatUserRoles::RoleMsg) {
        return userData.msg; // 返回消息
    }else if (role == ChatUserRoles::RoleTime) {
        return userData.lastMsgTime.toString("yyyy-MM-dd HH:mm:ss"); // 格式化时间
    }

    return QVariant();
}
