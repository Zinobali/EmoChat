#include "listitembase.h"

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
