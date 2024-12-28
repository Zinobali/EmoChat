#ifndef LISTITEMBASE_H
#define LISTITEMBASE_H

#include <QWidget>

enum ListItemType{
    CHAT_USER_ITEM, //聊天用户
    CONTACT_USER_ITEM, //联系人用户
    SEARCH_USER_ITEM, //搜索到的用户
    ADD_USER_TIP_ITEM, //提示添加用户
    INVALID_ITEM,  //不可点击条目
    GROUP_TIP_ITEM, //分组提示条目
    LINE_ITEM,  //分割线
    APPLY_FRIEND_ITEM, //好友申请
};

class ListItemBase : public QWidget
{
    Q_OBJECT
public:
    explicit ListItemBase(QWidget *parent = nullptr);



    ListItemType item_type() const;
    void setItem_type(ListItemType type);

private:
    ListItemType item_type_;

signals:
};

#endif // LISTITEMBASE_H
