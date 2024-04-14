#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStringList>
#include <QThreadPool>
#include "userlist.h"
#include "messagequeue.h"
#include "chattcpserver.h"
#include "sendmessage.h"
#include "msgsocket.h"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void initUserTable();//初始化表
    void selectUserTableFromDB();
    void initTcpServer();
    void initThreadPool();

public slots:

private:
    Ui::MainWindow *ui;
    UserList* m_userlist;
    ChatTcpServer* m_tcpServer;
    QThreadPool* m_threadPool;
};
#endif // MAINWINDOW_H
