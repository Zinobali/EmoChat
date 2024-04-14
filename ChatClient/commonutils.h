#ifndef COMMONUTILS_H
#define COMMONUTILS_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QByteArray>

class CommonUtils : public QObject
{
    Q_OBJECT
public:
    explicit CommonUtils(QObject *parent = nullptr);
    ~CommonUtils();

    static void writeIni(QString keystr, QString value);//写ini
    static QString readIni(QString keystr);//读ini
    static void clearIni();//清空ini
    static void initPath(QString fileName = "clientSettings.ini");//初始化当前路径
    static QString pwdEncode(const QString& originalData);
    static QString pwdDecode(const QString& encodingData);


private:
    static QString m_iniPath;//当前路径



signals:
};

#endif // COMMONUTILS_H
