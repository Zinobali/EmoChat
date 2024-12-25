#ifndef USERMGR_H
#define USERMGR_H

#include <QObject>
#include "singleton.h"

class UserMgr : public QObject, public Singleton<UserMgr>,
                public std::enable_shared_from_this<UserMgr>
{
    Q_OBJECT
    friend class Singleton<UserMgr>;
public:
    ~UserMgr();
    int uid() const;
    void setUid(int newUid);
    QString name() const;
    void setName(const QString &newName);
    QString token() const;
    void setToken(const QString &newToken);

private:
    explicit UserMgr();

private:
    int uid_;
    QString name_;
    QString token_;


signals:
};

#endif // USERMGR_H
