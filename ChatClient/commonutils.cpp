#include "commonutils.h"

//静态成员变量，类内声明，类外初始化。
QString CommonUtils::m_iniPath = "";

CommonUtils::CommonUtils(QObject *parent)
    : QObject{parent}
{

}

CommonUtils::~CommonUtils()
{

}

void CommonUtils::initPath(QString fileName)
{
    if(!m_iniPath.isEmpty())
    {
        return;
    }
    QDir path = QCoreApplication::applicationDirPath();
    if (path.dirName().toLower() == "debug"|| path.dirName().toLower() == "release"|| path.dirName().toLower() == "bin")
    {
        path.cdUp();
        path.cdUp();
        path.cd("ChatClient");
    }
    m_iniPath = QDir::toNativeSeparators(path.absolutePath().append("/").append(fileName));
}

QString CommonUtils::pwdEncode(const QString &originalData)
{

}

QString CommonUtils::pwdDecode(const QString &encodingdata)
{

}

void CommonUtils::writeIni(QString keystr, QString value)
{
    initPath();
    QSettings* ini = new QSettings(m_iniPath, QSettings::IniFormat);
    ini->beginGroup("UserInfo");
    ini->setValue(keystr,value);
    ini->endGroup();

    //写完删除
    delete ini;
}

QString CommonUtils::readIni(QString keystr)
{
    initPath();
    QSettings *ini = new QSettings(m_iniPath, QSettings::IniFormat);

    //检查文件是否存在
    if(!QFile::exists(m_iniPath))
    {
        ini->setValue("UserInfo/checkState","");
    }

    ini->beginGroup("UserInfo");
    QString value = ini->value(keystr).toString();
    ini->endGroup();

    //读入入完成后删除指针
    delete ini;

    return value;
}

void CommonUtils::clearIni()
{
    if (!m_iniPath.isEmpty())
    {
        // 删除配置文件
        QFile::remove(m_iniPath);
    }
}
