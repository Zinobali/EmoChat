#ifndef CHATPAGE_H
#define CHATPAGE_H

#include <QWidget>
#include "userdata.h"
#include <QMap>

constexpr int MAX_MSG_CONTENT_LEN = 1024;

namespace Ui
{
class ChatPage;
}
struct MsgInfo;
class ChatPage : public QWidget
{
    Q_OBJECT

public:
    explicit ChatPage(QWidget *parent = nullptr);
    ~ChatPage();
    void SetUserInfo(std::shared_ptr<UserInfo> user_info);
    void AppendChatMsg(std::shared_ptr<TextChatData> msg);

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private slots:
    void on_send_btn_clicked();

private:
    void initUI();
    void initSignals();
    void clearItems();
    void processMessage(const MsgInfo &msg, const std::shared_ptr<UserInfo>& user_info, QJsonArray &textArray, int &txt_size);
    QWidget *handleTextMessage(const MsgInfo &msg, const std::shared_ptr<UserInfo>& user_info, QJsonArray &textArray, int &txt_size);
    void sendChatData(int fromUid, int toUid, QJsonArray &textArray);

private:
    Ui::ChatPage *ui;
    std::shared_ptr<UserInfo> _user_info;
    QMap<QString, QWidget *> _bubble_map;

signals:
    void sig_append_send_chat_msg(std::shared_ptr<TextChatData> msg);
};

#endif // CHATPAGE_H
