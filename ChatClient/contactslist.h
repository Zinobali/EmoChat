#ifndef CONTACTSLIST_H
#define CONTACTSLIST_H

#include <QListWidget>
#include "conuseritem.h"

class ContactsList : public QListWidget
{
    Q_OBJECT
public:
    ContactsList(QWidget *parent = nullptr);
    void ShowRedPoint(bool bshow = true);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void addContactUserList();

private:
    ConUserItem *_add_friend_item;
    QListWidgetItem *_groupitem;

public slots:
    void slot_item_clicked(QListWidgetItem *item);
    //     void slot_add_auth_firend(std::shared_ptr<AuthInfo>);
    //     void slot_auth_rsp(std::shared_ptr<AuthRsp>);

signals:
    void sig_loading_contact_user();
    void sig_switch_apply_friend_page();
    void sig_switch_friend_info_page();
};

#endif // CONTACTSLIST_H
