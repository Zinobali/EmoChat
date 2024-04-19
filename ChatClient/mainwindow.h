#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <message.h>
#include <msgsocket.h>
#include "messageprocessor.h"
#include <QThread>
#include <QTableWidget>
#include "chatwidget.h"
#include <QFileDialog>
#include <QImage>
#include <QBuffer>
#include <memory>

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
    void loginSuccessSlot(User user, MsgSocket *socket);
    void initUserInfo();
    void processMessage(qint64 type, QByteArray data);
    void initThread();
    void sendOnlineMessage();

private:
    void initUI();
    
private slots:
    void textMessageSlot(int, QString);
    void updateFriendList(QVector<User> friends);
    void itemDoubleClickedSlot(QTableWidgetItem *item);
    void onItemDoubleClicked(const QModelIndex &index);
    void sendPushButtonSlot();
    void setReddot(int id);

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    MsgSocket *m_socket;
    std::shared_ptr<User> m_user;
    // User m_user;
    // QVector<User> m_friends;
    QPixmap m_defaultAvatar;
    QMultiMap<int, QString> m_messages;
};
#endif // MAINWINDOW_H
