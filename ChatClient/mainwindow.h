#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <message.h>
#include <msgsocket.h>
#include "messageprocessor.h"
#include <QThread>
#include <QTableWidget>
#include "friendlistitem.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void test();
    void loginSuccessSlot(User user,MsgSocket *socket);
    void initUserInfo();
    void processMessage(qint64 type, QByteArray data);
    void initThread();
    void sendOnlineMessage();

private:
    void textMessageSlot(int,QString);
    void updateFriendList(QVector<User> friends);
    void itemDoubleClickedSlot(QTableWidgetItem *item);
    void sendPushButtonSlot();
    void setReddot(int id);

private:
    Ui::MainWindow *ui;
    MsgSocket *m_socket;
    User m_user;
    QVector<User> m_friends;
    QPixmap m_defaultAvatar;
    QMultiMap<int,QString> m_messages;
};
#endif // MAINWINDOW_H
