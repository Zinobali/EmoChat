#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QPointF>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QPixmap>
#include "msgsocket.h"
#include "message.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    //functions
    void SignIn();
    void shakeWindow();//窗口晃动
    virtual bool eventFilter(QObject *watched, QEvent *event) override;//事件过滤器
    void goToSignUp();//去注册
    void clearInput();//清空输入框
    bool connectToChatServer();//连接服务器
    void sendSignInMessage();//发送登录消息
    void handleMessage(); //登录

private slots:
    void ChangeRemberStateSlots();
    void on_closeBtn_clicked();
    void processMessage(qint64 type, QByteArray data);

signals:
    void enterKeyPressed();
    void signUpLabelClicked();
    void loginSuccess(User user,MsgSocket *socket);

private:
    void readIni();
    void writeIni();

protected:
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;

private:
    Ui::LoginDialog *ui;
    QPointF m_Pos;   //开始拖动的位置
    QPixmap m_Icon; //窗口图标
    bool m_IsRemember;  //记住我
    MsgSocket* m_socket; //不要提前释放，要交给mainwindow管理
};

#endif // LOGINDIALOG_H
