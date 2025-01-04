#include "conuseritem.h"
#include "ui_conuseritem.h"

ConUserItem::ConUserItem(QWidget *parent)
    : ListItemBase(parent), ui(new Ui::ConUserItem)
{
    ui->setupUi(this);
    setItem_type(ListItemType::CONTACT_USER_ITEM);
    ui->red_point->raise();
    ShowRedPoint(true);
}

ConUserItem::~ConUserItem()
{
    delete ui;
}

QSize ConUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}

void ConUserItem::SetInfo(int uid, QString name, QString icon)
{
    _info = std::make_shared<UserInfo>(uid, name, icon);
    ui->user_name_lb->setText(_info->_name);
    // 加载图片
    QPixmap pixmap(_info->_icon);
    // 设置图片自动缩放
    ui->icon_lb->setPixmap(pixmap.scaled(ui->icon_lb->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    ui->icon_lb->setScaledContents(true);
}

void ConUserItem::ShowRedPoint(bool show)
{
    ui->red_point->setVisible(show);
}
