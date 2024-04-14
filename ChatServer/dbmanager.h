#ifndef DBMANAGER_H
#define DBMANAGER_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QDebug>

class DBManager : public QObject
{
    Q_OBJECT
public:
    explicit DBManager(QObject *parent = nullptr);

    static void connectMysql();
    static void closeMysql();

public:
    static QSqlDatabase db;

};

#endif // DBMANAGER_H
