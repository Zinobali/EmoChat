#include "chatuserwidget.h"
#include "ui_chatuserwidget.h"

ChatUserWidget::ChatUserWidget(QWidget *parent)
    : ListItemBase(parent)
    , ui(new Ui::ChatUserWidget)
{
    ui->setupUi(this);
    setItem_type(ListItemType::CHAT_USER_ITEM);
}

ChatUserWidget::~ChatUserWidget()
{
    delete ui;
}

void ChatUserWidget::SetInfo(QString name, QString head, QString msg)
{
    name_ = name;
    head_ = head;
    msg_ = msg;
    // 头像
    QPixmap pixmap(head_);
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    // 昵称
    ui->user_name_lb->setText(name_);
    ui->user_chat_lb->setText(msg_);
}

QSize ChatUserWidget::sizeHint() const
{
    return QSize(300, 70);
}
