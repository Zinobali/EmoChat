#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QDateTime>
#include <QByteArray>
#include <QDataStream>
#include <QIODevice>
#include <QFile>
#include "jsonmanager.h"
#include "user.h"
// #include "userlist.h"

class Message : public QObject
{
    Q_OBJECT
public:
    enum TYPE{
        TEXT_MSG,
        IMAGE_MSG,
        FILE_MSG,

        SIGN_IN,
        SIGN_IN_REPLY,
        SIGN_UP,
        ONLINE,
        ONLINE_REPLY,
        SIGN_OUT
    };

    constexpr static int SYSTEM_ID = 0;
    explicit Message(QObject* parent = nullptr);

    virtual int senderId() const;
    virtual int receiverId() const;
    virtual TYPE type() const = 0;
    virtual QByteArray serialize() const = 0;
    virtual void deserialize(const QByteArray &data) = 0;

protected:
    int m_senderId;
    int m_receiverId;
};

// 文本消息类 TextMessage
class TextMessage : public Message {
    Q_OBJECT
public:
    explicit TextMessage(QObject *parent = nullptr);
    explicit TextMessage(const int &senderId, const int &receiverId, const QString &text, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;
    QString text() const;

private:
    QString m_text;
};

// 图片消息类 ImageMessage
class ImageMessage : public Message {
    Q_OBJECT
public:
    explicit ImageMessage(QObject *parent = nullptr);
    explicit ImageMessage(const int &senderId, const int &receiverId, const QByteArray &imageData, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;

private:
    QByteArray m_imageData;
};

// 文件消息类 FileMessage
class FileMessage : public Message {
    Q_OBJECT
public:
    explicit FileMessage(QObject *parent = nullptr);
    explicit FileMessage(const int &senderId, const int &receiverId, const QString &fileName, const QByteArray &fileData, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;

private:
    QString m_fileName;
    QByteArray m_fileData;
};

// 登录消息类 SignInMessage
class SignInMessage : public Message {
    Q_OBJECT
public:
    explicit SignInMessage(QObject *parent = nullptr);
    explicit SignInMessage(const int &senderId, const int &receiverId, const QString &password, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;
    QString password() const;

private:
    QString m_password;
};

// 登陆消息回执
class SignInReplyMessage : public Message {
    Q_OBJECT
public:
    explicit SignInReplyMessage(QObject *parent = nullptr);
    explicit SignInReplyMessage(const int &senderId, const int &receiverId, const bool &isValid, const User* user, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;
    bool isValid() const;
    User user() const;

private:
    bool m_isValid;
    User m_user;
};

// 上线消息类 onlineMessage
class onlineMessage : public Message {
    Q_OBJECT
public:
    explicit onlineMessage(QObject *parent = nullptr);
    explicit onlineMessage(const int &senderId, const int &receiverId, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;
};

// 上线回执类 onlineReplyMessage
class onlineReplyMessage : public Message {
    Q_OBJECT
public:
    explicit onlineReplyMessage(QObject *parent = nullptr);
    explicit onlineReplyMessage(const int &senderId, const int &receiverId, QObject *parent = nullptr);

    virtual TYPE type() const override;
    virtual QByteArray serialize() const override;
    virtual void deserialize(const QByteArray &data) override;
    QVector<User> friends() const;

private:
    QVector<User> m_friends;
};

// 消息工厂类 MessageFactory
class MessageFactory 
{
public:
    static Message* createMessage(const Message::TYPE &type,const QByteArray &data);
};

#endif  //MESSAGE_H
