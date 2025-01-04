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

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void initUI();
    void initSignals();
    void addChatUserList();
    void setSideBarUserHead(const QString &headUrl);
    void clearOtherLabelState(StateWidget *w);
    void handleGlobalMousePress(QMouseEvent *event);

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
};

#endif // CHATDIALOG_H
