#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include "userdata.h"
#include <QMap>
#include <QListWidgetItem>

enum ChatUIMode
{
    SearchMode,  // 搜索模式
    ChatMode,    // 聊天模式
    ContactMode, // 联系模式
};

namespace Ui
{
    class ChatDialog;
}

class StateWidget;
class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void ShowSearch(bool show);
    void AddLabelGroup(StateWidget *label);
    void UpdateChatMsg(std::vector<std::shared_ptr<TextChatData>> msgdata);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    void initSignals();
    void addChatUserList(); // 添加聊天用户列表
    void setSideBarUserHead(const QString &headUrl);
    void clearOtherLabelState(StateWidget *w);
    void handleGlobalMousePress(QMouseEvent *event);
    void setSelectedChatItem(int uid);
    void setSelectedChatPage(int uid);
    void loadMoreChatUser();
    void loadMoreContacts();

private slots:
    void slot_loading_chat_user();
    void slot_loading_contact_user();
    void slot_search_text_changed(const QString &text);
    // void slot_sidebar_widget_clicked(StateWidget *label);
    void slot_side_chat_clicked();
    void slot_side_contact_clicked();
    void slot_friend_info_page(std::shared_ptr<UserInfo> user_info);
    void slot_switch_apply_friend_page();
    void slot_jump_chat_item_from_friend_info_page(std::shared_ptr<UserInfo> user_info);
    void slot_chat_item_clicked(QListWidgetItem *item);
    void slot_append_send_chat_msg(std::shared_ptr<TextChatData> msg);
    void slot_text_chat_msg(std::shared_ptr<TextChatMsg> msg);

public slots:
    void slot_friend_apply(std::shared_ptr<AddFriendApply> apply); // 收到好友申请消息
    void slot_add_auth_firend(std::shared_ptr<AuthInfo> auth_info);
    void slot_auth_rsp(std::shared_ptr<AuthRsp> auth_rsp);
    void slot_jump_chat_item(std::shared_ptr<SearchInfo> si);

private:
    Ui::ChatDialog *ui;
    ChatUIMode mode_;
    ChatUIMode state_;
    bool b_loading_;
    QList<StateWidget *> label_list_;
    QWidget *last_widget_;
    QMap<int, QListWidgetItem *> chat_items_map_;
    int cur_chat_uid_;
};

#endif // CHATDIALOG_H
