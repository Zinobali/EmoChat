#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListWidget>

class ChatUserList : public QListWidget
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:


signals:
    void sig_loading_chat_user();
};

#endif // CHATUSERLIST_H
