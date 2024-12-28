#ifndef CHATUSERDELEGATE_H
#define CHATUSERDELEGATE_H

#include <QStyledItemDelegate>

class ChatUserDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    ChatUserDelegate(QObject *parent = nullptr);
    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    QString parseTime(const QDateTime &time) const;
};

#endif // CHATUSERDELEGATE_H
