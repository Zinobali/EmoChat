#include "chatuseritem.h"
#include "ui_chatuseritem.h"

ChatUserItem::ChatUserItem(QWidget *parent)
    : ListItemBase(parent), ui(new Ui::ChatUserItem)
{
    ui->setupUi(this);
    setItem_type(ListItemType::CHAT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(false);
}

ChatUserItem::~ChatUserItem()
{
    delete ui;
}

QSize ChatUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ChatUserItem::ShowRedPoint(bool bshow)
{
    ui->red_point->setVisible(bshow);
}

void ChatUserItem::SetInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_user_info->_name);
    ui->user_chat_lb->setText(_user_info->_last_msg);
}

void ChatUserItem::SetInfo(std::shared_ptr<FriendInfo> friend_info)
{
    _user_info = std::make_shared<UserInfo>(friend_info);
    // 加载图片
    QPixmap pixmap(_user_info->_icon);

    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);

    ui->user_name_lb->setText(_user_info->_name);
    ui->user_chat_lb->setText(_user_info->_last_msg);
}

std::shared_ptr<UserInfo> ChatUserItem::GetUserInfo()
{
    return _user_info;
}

void ChatUserItem::UpdateLastMsg(std::vector<std::shared_ptr<TextChatData> > msgs)
{
    QString last_msg = "";
    for (auto& msg : msgs) {
        last_msg = msg->_msg_content;
        _user_info->_chat_msgs.push_back(msg);
    }

    _user_info->_last_msg = last_msg;
    ui->user_chat_lb->setText(_user_info->_last_msg);
}
