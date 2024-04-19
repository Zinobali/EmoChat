#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbmanager.h"

#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    DBManager::connectMysql();
    initUserTable();
    initTcpServer();
    initThreadPool();
    setWindowTitle("通讯中转服务器");
    // initMessageQueue();
}

MainWindow::~MainWindow()
{
    delete ui;
    DBManager::closeMysql(); // 关闭数据库
    m_userlist->deleteLater();
    m_threadPool->deleteLater();

    // 关闭m_clients中的连接，并delete
    //  for (int i = 0; i < m_clients.size(); ++i)
    //  {
    //      MsgSocket* client = m_clients.at(i);
    //      if(client != nullptr)
    //      {
    //          delete client;
    //      }
    //  }
    //  m_clients.clear();
    // 最后关闭tcpServer
    //  m_tcpServer->close();
    m_tcpServer->deleteLater();
}

void MainWindow::initUserTable()
{
    // 表格样式
    QStringList headerLabels;
    headerLabels << "ID"
                 << "用户名"
                 << "在线状态";
    ui->tableWidget->setColumnCount(headerLabels.count());
    ui->tableWidget->setHorizontalHeaderLabels(headerLabels);
    ui->tableWidget->verticalHeader()->setVisible(false);
    ui->tableWidget->horizontalHeader()->setStyleSheet("font-weight: bold;");
    ui->tableWidget->setAlternatingRowColors(true);
    selectUserTableFromDB();
}

void MainWindow::selectUserTableFromDB()
{
    // ui->tableWidget->clearContents();
    // 查询用户
    m_userlist = UserList::getInstance();
    const QSet<User *> *const users = m_userlist->getUsers();
    for (QSet<User *>::const_iterator it = users->constBegin(); it != users->constEnd(); it++)
    {
        // 在表格中插入数据
        int row = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(row);
        ui->tableWidget->setItem(row, 0, new QTableWidgetItem(QString::number((*it)->id())));
        ui->tableWidget->setItem(row, 1, new QTableWidgetItem((*it)->username()));
        ui->tableWidget->setItem(row, 2, new QTableWidgetItem("离线"));
    }
}

void MainWindow::initTcpServer()
{
    m_tcpServer = ChatTcpServer::getInstance();
    m_tcpServer->listen(QHostAddress::Any, 45678);
    connect(m_tcpServer, &ChatTcpServer::userOnline,[=](int uid){
        //改变表格的状态
        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            if(ui->tableWidget->item(i, 0)->text().toInt() == uid){
                ui->tableWidget->setItem(i, 2, new QTableWidgetItem("在线"));
            }
        }
    });
    connect(m_tcpServer, &ChatTcpServer::userOffline, m_userlist, &UserList::setOfflineUser);
    connect(m_tcpServer, &ChatTcpServer::userOffline,[=](int uid){
        //改变表格的状态
        for(int i = 0; i < ui->tableWidget->rowCount(); i++){
            if(ui->tableWidget->item(i, 0)->text().toInt() == uid){
                ui->tableWidget->setItem(i, 2, new QTableWidgetItem("离线"));
            }
        }
    });
}

void MainWindow::initThreadPool()
{
    m_threadPool = QThreadPool::globalInstance();
    m_threadPool->setMaxThreadCount(3);
}

void MainWindow::on_pushButton_clicked()
{
    //文件对话框选择图片格式为jpg
    QString pictureName = QFileDialog::getOpenFileName(this, "选择图片", "", "Image Files(*.jpg)");
    if (pictureName.isEmpty())
    {
        return;
    }
    QImage image(pictureName);
    QByteArray picData;
    QBuffer buffer(&picData);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "JPG");
    qDebug() << picData;
    //头像写入数据库
    QSqlQuery query;
    QVariant picDataVar(picData);
    QString sql = "UPDATE `user` SET avatar = :picture where id = :id";
    query.prepare(sql);
    query.bindValue(":picture", picDataVar);
    query.bindValue(":id", 2);
    if (query.exec())
    {
        qDebug() << "头像写入数据库成功";
    }
    else
    {
        qDebug() << "头像写入数据库失败";
    }
}

