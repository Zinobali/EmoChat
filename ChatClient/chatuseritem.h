#ifndef CHATUSERITEM_H
#define CHATUSERITEM_H

#include "listitembase.h"
#include "userdata.h"

namespace Ui
{
    class ChatUserItem;
}

class ChatUserItem : public ListItemBase
{
    Q_OBJECT

public:
    explicit ChatUserItem(QWidget *parent = nullptr);
    ~ChatUserItem();
    virtual QSize sizeHint() const override;
    void ShowRedPoint(bool bshow = true);
    void SetInfo(std::shared_ptr<UserInfo> user_info);
    void SetInfo(std::shared_ptr<FriendInfo> friend_info);
    std::shared_ptr<UserInfo> GetUserInfo();
    void UpdateLastMsg(std::vector<std::shared_ptr<TextChatData>> msgs);

private:
    Ui::ChatUserItem *ui;
    std::shared_ptr<UserInfo> _user_info;
};

#endif // CHATUSERITEM_H
