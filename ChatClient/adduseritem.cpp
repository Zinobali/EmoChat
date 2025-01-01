#include "adduseritem.h"
#include "ui_adduseritem.h"

AddUserItem::AddUserItem(QWidget *parent)
    : ListItemBase(parent), ui(new Ui::AddUserItem)
{
    ui->setupUi(this);
    setItem_type(ListItemType::ADD_USER_TIP_ITEM);
}

AddUserItem::~AddUserItem()
{
    delete ui;
}

QSize AddUserItem::sizeHint() const
{
    return QSize(250, 70); // 返回自定义的尺寸
}
