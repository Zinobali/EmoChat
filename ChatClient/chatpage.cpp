#include "chatpage.h"
#include "ui_chatpage.h"
#include "chatitembase.h"
#include <QPainter>
#include <QStyleOption>
#include "textbubble.h"
#include "picturebubble.h"

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

void ChatPage::on_send_btn_clicked()
{
    auto *pTextEdit = ui->chat_edit;
    ChatRole role = ChatRole::Self;
    QString userName = QStringLiteral("旻山君");
    QString userIcon = ":/images/head_1.png";

    const auto &msgList = pTextEdit->getMsgList();
    for (int i = 0; i < msgList.size(); ++i)
    {
        auto type = msgList[i].msgFlag;
        ChatItemBase *pChatItem = new ChatItemBase(role);
        pChatItem->SetUserName(userName);
        pChatItem->SetUserIcon(QPixmap(userIcon));
        QWidget *pBubble = nullptr;
        if (type == MsgType::Text)
        {
            pBubble = new TextBubble(role, msgList[i].content);
        }
        else if (type == MsgType::Image)
        {
            pBubble = new PictureBubble(QPixmap(msgList[i].content), role);
        }
        else if (type == MsgType::File)
        {
        }

        if (pBubble)
        {
            pChatItem->SetBubble(pBubble);
            ui->chat_data_list->AppendChatItem(pChatItem);
        }
    }
}
