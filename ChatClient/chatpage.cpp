#include "chatpage.h"
#include "ui_chatpage.h"
#include "chatitembase.h"
#include <QPainter>
#include <QStyleOption>
#include "textbubble.h"
#include "picturebubble.h"
#include "usermgr.h"
#include <QJsonArray>
#include <QUuid>
#include "tcpmgr.h"
#include <QJsonDocument>

ChatPage::ChatPage(QWidget *parent)
    : QWidget(parent), ui(new Ui::ChatPage)
{
    ui->setupUi(this);
    initUI();
    initSignals();
}

ChatPage::~ChatPage()
{
    delete ui;
}

void ChatPage::SetUserInfo(std::shared_ptr<UserInfo> user_info)
{
    _user_info = user_info;
    // 设置ui界面
    ui->title_lb->setText(_user_info->_name);
    ui->chat_data_list->removeAllItem();
    for (auto &msg : user_info->_chat_msgs)
    {
        AppendChatMsg(msg);
    }
}

void ChatPage::AppendChatMsg(std::shared_ptr<TextChatData> msg)
{
    auto self_info = UserMgr::GetInstance()->GetUserInfo();
    ChatRole role;
    if (msg->_from_uid == self_info->_uid)
    {
        role = ChatRole::Self;
        // 自己信息
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(self_info->_name);
        pChatItem->SetUserIcon(QPixmap(self_info->_icon));
        QWidget *pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->SetBubble(pBubble); // 消息气泡
        ui->chat_data_list->AppendChatItem(pChatItem);
    }
    else
    {
        role = ChatRole::Other;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        auto friend_info = UserMgr::GetInstance()->GetFriendById(msg->_from_uid);
        if (!friend_info)
        {
            return;
        }
        pChatItem->SetUserName(friend_info->_name);
        pChatItem->SetUserIcon(QPixmap(friend_info->_icon));
        QWidget *pBubble = new TextBubble(role, msg->_msg_content);
        pChatItem->SetBubble(pBubble);
        ui->chat_data_list->AppendChatItem(pChatItem);
    }
}

void ChatPage::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void ChatPage::initUI()
{
    // 设置按钮样式
    ui->receive_btn->SetState("normal", "hover", "press");
    ui->send_btn->SetState("normal", "hover", "press");
    // 设置图标样式
    ui->emo_lb->SetState("normal", "hover", "press", "normal", "hover", "press");
    ui->file_lb->SetState("normal", "hover", "press", "normal", "hover", "press");
}

void ChatPage::initSignals()
{
    connect(ui->chat_edit, &MsgTextEdit::sig_send_msg, this, &ChatPage::on_send_btn_clicked);
}

void ChatPage::clearItems()
{
    ui->chat_data_list->removeAllItem();
}

void ChatPage::processMessage(const MsgInfo &msg, const std::shared_ptr<UserInfo> &user_info, QJsonArray &textArray, int &txt_size)
{
    ChatRole role = ChatRole::Self;
    QString userName = user_info->_name;
    QString userIcon = user_info->_icon;

    ChatItemBase *pChatItem = new ChatItemBase(role);
    pChatItem->SetUserName(userName);
    pChatItem->SetUserIcon(QPixmap(userIcon));
    QWidget *pBubble = nullptr;
    auto msgType = msg.msgFlag;

    if (msgType == MsgType::Text)
    {
        pBubble = handleTextMessage(msg, user_info, textArray, txt_size);
    }
    else if (msgType == MsgType::Image)
    {
        pBubble = new PictureBubble(QPixmap(msg.content), role);
    }
    else if (msgType == MsgType::File)
    {
        // Handle file messages if needed
    }

    if (pBubble)
    {
        pChatItem->SetBubble(pBubble);
        ui->chat_data_list->AppendChatItem(pChatItem);
    }
}

QWidget *ChatPage::handleTextMessage(const MsgInfo &msg, const std::shared_ptr<UserInfo> &user_info, QJsonArray &textArray, int &txt_size)
{
    QWidget *pBubble = new TextBubble(ChatRole::Self, msg.content);

    if (txt_size + msg.content.length() > MAX_MSG_CONTENT_LEN)
    {
        sendChatData(user_info->_uid, _user_info->_uid, textArray);
        txt_size = 0;
        textArray = QJsonArray();
    }

    QUuid uuid = QUuid::createUuid();
    QString uuid_str = uuid.toString();
    QJsonObject obj;
    QByteArray utf8Message = msg.content.toUtf8();
    obj["content"] = QString::fromUtf8(utf8Message);
    obj["msgid"] = uuid_str;
    textArray.append(obj);

    auto txt_msg = std::make_shared<TextChatData>(uuid_str, utf8Message, user_info->_uid, _user_info->_uid);
    emit sig_append_send_chat_msg(txt_msg);

    txt_size += msg.content.length();
    return pBubble;
}

void ChatPage::sendChatData(int fromUid, int toUid, QJsonArray &textArray)
{
    QJsonObject textObj;
    textObj["fromuid"] = fromUid;
    textObj["touid"] = toUid;
    textObj["text_array"] = textArray;

    QJsonDocument doc(textObj);
    QByteArray jsonData = doc.toJson(QJsonDocument::Compact);

    emit TcpMgr::GetInstance()->sig_send_data(RequestId::ID_TEXT_CHAT_MSG_REQ, jsonData);
}

void ChatPage::on_send_btn_clicked()
{
    if (_user_info == nullptr)
    {
        qDebug() << "friend_info is empty";
        return;
    }

    auto user_info = UserMgr::GetInstance()->GetUserInfo();
    const auto &msgList = ui->chat_edit->getMsgList();

    QJsonArray textArray;
    int txt_size = 0;

    for (const auto &msg : msgList)
    {
        if (msg.content.length() > MAX_MSG_CONTENT_LEN)
        {
            qDebug() << "msg content is too long";
            continue;
        }

        processMessage(msg, user_info, textArray, txt_size);
    }

    sendChatData(user_info->_uid, _user_info->_uid, textArray);
}
