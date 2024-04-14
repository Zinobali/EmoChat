#ifndef FRIENDLISTITEM_H
#define FRIENDLISTITEM_H

#include <QWidget>
#include <QMouseEvent>

namespace Ui {
class FriendListItem;
}

class FriendListItem : public QWidget
{
    Q_OBJECT

public:
    explicit FriendListItem(QString name, QString avatar, QWidget *parent = nullptr);
    ~FriendListItem();
    // void setFriendName(QString name);
    // void setFriendAvatar(QString avatar);

    int id() const;
    void setId(int newId);

    QString username() const;
    void setUsername(const QString &newUsername);

signals:
    void doubleClicked(FriendListItem *item);

private:
    Ui::FriendListItem *ui;
    int m_id;
    QString m_username;

protected:
    //双击事件
    virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
};

#endif // FRIENDLISTITEM_H
