#include "mainwindow.h"
#include "logindialog.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    // 注册自定义类型
    qRegisterMetaType<User>("User");
    qRegisterMetaType<MsgSocket*>("MsgSocket*");
    QApplication a(argc, argv);
    LoginDialog loginDialog;
    //提前创建MainWindow对象并连接槽函数,给MainWindow传入User、socket
    //1.这里可以使用信号槽
    //2.或者使用友元，让LoginDialog类访问MainWindow的私有属性
    MainWindow w;
    QObject::connect(&loginDialog, &LoginDialog::loginSuccess, &w, &MainWindow::loginSuccessSlot);
    if(loginDialog.exec() == QDialog::Accepted)
    {
        w.show();
        return a.exec();
    }
    return 0;
}
