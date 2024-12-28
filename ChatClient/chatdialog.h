#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <vector>

enum ChatUIMode{
    SearchMode, //搜索模式
    ChatMode, //聊天模式
    ContactMode, //联系模式
};

namespace Ui {
class ChatDialog;
}

class ChatDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ChatDialog(QWidget *parent = nullptr);
    ~ChatDialog();
    void ShowSearch(bool show);

private:
    void initUI();
    void initSignals();
    void addChatUserList();

private slots:
    void slot_loading_chat_user();

private:
    Ui::ChatDialog *ui;
    ChatUIMode mode_;
    ChatUIMode state_;
    bool b_loading_;

    static std::vector<QString> strs;
    static std::vector<QString> heads;
    static std::vector<QString> names;

};

#endif // CHATDIALOG_H
