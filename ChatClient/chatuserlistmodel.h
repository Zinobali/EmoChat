#ifndef CHATUSERLISTMODEL_H
#define CHATUSERLISTMODEL_H

#include <QAbstractListModel>
#include <QDateTime>

enum ChatUserRoles {
    RoleName = Qt::UserRole + 1,    //自定义角色：用户名称
    RoleHead = Qt::UserRole + 2,    // 自定义角色：头像
    RoleMsg = Qt::UserRole + 3,     // 自定义角色：消息
    RoleTime = Qt::UserRole + 4     // 自定义角色：最后消息的时间
};

struct ChatUser
{
    QString name;
    QString head;
    QString msg;
    QDateTime lastMsgTime;  // 新增字段：最后一次消息的时间
};

class ChatUserListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ChatUserListModel(QObject *parent = nullptr);
    void AddChatUser(const QString& name, const QString& headUrl, const QString& msg, const QDateTime& lastMsgTime);
    void AddChatUsers(const QVector<ChatUser>& users);

    /*
     * 模板函数不要写在cpp文件，否则需要显示实例化
     * 显式实例化要求在 .cpp 文件中为每一种想要支持的类型明确指定模板的实例化。
     */
    template<typename InputIterator>
    void AddChatUsers(InputIterator begin, InputIterator end)
    {
        QVector<ChatUser> newUsers(begin, end);
        AddChatUsers(newUsers);
    }

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

signals:
    void sig_loading_started();
    void sig_loading_finished();

private:
    QVector<ChatUser> users_;
};

#endif // CHATUSERLISTMODEL_H
