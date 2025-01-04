#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "singleton.h"

class FriendInfo;
class ApplyInfo;

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

    std::vector<std::shared_ptr<ApplyInfo> > apply_list() const;
    void setApply_list(const std::vector<std::shared_ptr<ApplyInfo> > &newApply_list);
    std::vector<std::shared_ptr<FriendInfo> > friend_list() const;
    void setFriend_list(const std::vector<std::shared_ptr<FriendInfo> > &newFriend_list);

private:
    explicit UserMgr();
    void test_function(); // 测试函数，后续删除

private:
    int uid_;
    QString name_;
    QString token_;
    std::vector<std::shared_ptr<ApplyInfo>> apply_list_;   // 好友申请列表
    std::vector<std::shared_ptr<FriendInfo>> friend_list_; // 好友列表

signals:
};

#endif // USERMGR_H
