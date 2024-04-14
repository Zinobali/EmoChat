#include "dbmanager.h"

QSqlDatabase DBManager::db;

DBManager::DBManager(QObject *parent)
    : QObject{parent}
{}

void DBManager::connectMysql()
{
    // 添加 MySQL 数据库驱动
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost"); // MySQL 服务器主机名
    db.setPort(2897);
    db.setUserName("root"); // MySQL 用户名
    db.setPassword("123456"); // MySQL 密码
    db.setDatabaseName("HappyChat"); // 数据库名称

    // 打开数据库连接
    if (db.open()) {
        qDebug() << "数据库连接成功!";

    } else {
        qWarning() << "错误:" << db.lastError().text();
    }
}

void DBManager::closeMysql()
{
    db.close(); // 关闭数据库连接
    qDebug() << "关闭数据库连接!";
}
