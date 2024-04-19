#ifndef FRIENDLISTVIEWDELEGATE_H
#define FRIENDLISTVIEWDELEGATE_H

#include <QWidget>
#include <QStyledItemDelegate>
#include <QPainter>
#include "user.h"
#include <memory>

class FriendListDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit FriendListDelegate(QWidget *parent = nullptr);

signals:



    // QAbstractItemDelegate interface
public:
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
};

#endif // FRIENDLISTVIEWDELEGATE_H
