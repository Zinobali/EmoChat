#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "singleton.h"
#include <QHash>
#include <QMap>
#include <vector>
#include "userdata.h"
#include <QJsonArray>

class UserMgr : public QObject, public Singleton<UserMgr>, public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;

public:
    ~UserMgr();
    int uid() const;
    void setUid(int newUid);
    QString name() const;
    void setName(const QString &newName);
    QString token() const;
    void setToken(const QString &newToken);
    bool CheckFriendById(int uid);                       // 判断是否为好友
    bool AlreadyApply(int uid);                          // 判断是否已经在 申请列表中
    void AddApplyList(std::shared_ptr<ApplyInfo> apply); // 添加单个好友申请
    void SetUserInfo(const std::shared_ptr<UserInfo> &user_info);
    std::shared_ptr<UserInfo> GetUserInfo();
    std::vector<std::shared_ptr<ApplyInfo>> apply_list() const;   // 返回好友申请列表
    std::vector<std::shared_ptr<FriendInfo>> friend_list() const; // 返回好友列表
    void AppendApplyList(QJsonArray apply_array);

    void AddFriend(std::shared_ptr<AuthInfo> auth_info);
    void AddFriend(std::shared_ptr<AuthRsp> auth_rsp);
    std::shared_ptr<FriendInfo> GetFriendById(int uid);

private:
    explicit UserMgr();

private:
    std::shared_ptr<UserInfo> user_info_;
    QString token_;

    QMap<int, std::shared_ptr<ApplyInfo>> apply_map_;   // 好友申请列表
    QMap<int, std::shared_ptr<FriendInfo>> friend_map_; // 好友列表

signals:
};

#endif // USERMGR_H
