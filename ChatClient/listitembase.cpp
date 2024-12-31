#include "listitembase.h"

#include <QPainter>
#include <QStyleOption>

ListItemBase::ListItemBase(QWidget *parent)
    : QWidget{parent}
{}

ListItemType ListItemBase::item_type() const
{
    return item_type_;
}

void ListItemBase::setItem_type(ListItemType type)
{
    item_type_ = type;
}

void ListItemBase::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
