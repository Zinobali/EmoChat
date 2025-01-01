#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <QDialog>
#include <vector>

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

private:
    void initUI();
    void initSignals();
    void addChatUserList();
    void setSideBarUserHead(const QString &headUrl);
    void clearOtherLabelState(StateWidget *w);

private slots:
    void slot_loading_chat_user();
    void slot_search_text_changed(const QString &text);
    // void slot_sidebar_widget_clicked(StateWidget *label);
    void slot_side_chat_clicked();
    void slot_side_contact_clicked();

private:
    Ui::ChatDialog *ui;
    ChatUIMode mode_;
    ChatUIMode state_;
    bool b_loading_;
    QList<StateWidget *> label_list_;
    QWidget *last_widget_;

    static std::vector<QString> strs;
    static std::vector<QString> heads;
    static std::vector<QString> names;
};

#endif // CHATDIALOG_H
