#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT
public:
    User(const User &other);//拷贝构造函数
    explicit User(QObject *parent = nullptr);
    explicit User(int id, QString username, QString password, QByteArray avatar, QObject *parent = nullptr);

    // getters and setters
    int id() const;
    void setId(int newId);
    QString username() const;
    void setUsername(const QString &newUsername);
    // QString password() const;
    // void setPassword(const QString &newPassword);
    QByteArray avatar() const;
    void setAvatar(const QByteArray &newAvatar);
    // 检查密码
    bool checkPassword(const QString &password);
    //赋值运算符
    User &operator=(const User &other);
    //等于运算符
    bool operator==(const User &other) const;

private:
    int m_id;
    QString m_username;
    QString m_password;
    QByteArray m_avatar; // 头像
};

Q_DECLARE_METATYPE(User)

#endif // USER_H
