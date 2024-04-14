#include "friendlistitem.h"
#include "ui_friendlistitem.h"

FriendListItem::FriendListItem(QString name, QString avatar, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FriendListItem)
    , m_id(-1)
    , m_username(name)
{
    ui->setupUi(this);
    ui->friendName->setText(name);
    ui->friendAvatar->setPixmap(QPixmap(avatar).scaled(60,60));
    //ui->friendName文字水平居中
    ui->friendName->setAlignment(Qt::AlignCenter);
}

FriendListItem::~FriendListItem()
{
    delete ui;
}

int FriendListItem::id() const
{
    return m_id;
}

void FriendListItem::setId(int newId)
{
    m_id = newId;
}

QString FriendListItem::username() const
{
    return m_username;
}

void FriendListItem::setUsername(const QString &newUsername)
{
    m_username = newUsername;
}

void FriendListItem::mouseDoubleClickEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        emit doubleClicked(this);
    }
}

// void FriendListItem::mouseDoubleClickEvent(QMouseEvent *event)
// {
//
// }

// void FriendListItem::setFriendName(QString name)
// {
//     ui->friendName->setText(name);
// }

// void FriendListItem::setFriendAvatar(QString avatar)
// {
//     ui->friendAvatar->setPixmap(QPixmap(avatar).scaled(60,60));
// }
