#ifndef CHATUSERLIST_H
#define CHATUSERLIST_H

#include <QListView>
#include "chatuserdelegate.h"
#include "chatuserlistmodel.h"

class ChatUserList : public QListView
{
    Q_OBJECT
public:
    ChatUserList(QWidget *parent = nullptr);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    void initCustomScrollBar();

private:
    ChatUserListModel* model_;
    ChatUserDelegate* delegate_;
    QScrollBar* scrollBar_; // 自定义滚动条

private slots:
    void slot_showLoadingAnimation();
    void slot_hideLoadingAnimation();

    void slot_range_change(int min, int max);

signals:
    void sig_loading_chat_user();
    void sig_clicked();
};

#endif // CHATUSERLIST_H
