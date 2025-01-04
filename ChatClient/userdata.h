#ifndef USERDATA_H
#define USERDATA_H

#include <QString>
#include <memory>
#include <QJsonArray>
#include <vector>
#include <QJsonObject>

struct UserInfo
{

    UserInfo(int uid, const QString &name, const QString &icon)
        : _uid(uid), _name(name), _icon(icon) {}
    UserInfo(int uid, QString name, QString nick, QString icon, int sex, QString last_msg = "")
        : _uid(uid), _name(name), _nick(nick), _icon(icon), _sex(sex), _last_msg(last_msg) {}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
    QString _last_msg;
    // std::vector<std::shared_ptr<TextChatData>> _chat_msgs;
};

class AddFriendApply
{
public:
    AddFriendApply(int from_uid, QString name, QString desc,
                   QString icon, QString nick, int sex);
    int _from_uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
};

struct ApplyInfo
{
    ApplyInfo(int uid, QString name, QString desc,
              QString icon, QString nick, int sex, int status)
        : _uid(uid), _name(name), _desc(desc),
          _icon(icon), _nick(nick), _sex(sex), _status(status) {}

    ApplyInfo(std::shared_ptr<AddFriendApply> addinfo)
        : _uid(addinfo->_from_uid), _name(addinfo->_name),
          _desc(addinfo->_desc), _icon(addinfo->_icon),
          _nick(addinfo->_nick), _sex(addinfo->_sex),
          _status(0)
    {
    }
    void SetIcon(QString head)
    {
        _icon = head;
    }
    int _uid;
    QString _name;
    QString _desc;
    QString _icon;
    QString _nick;
    int _sex;
    int _status;
};

struct AuthRsp
{
    AuthRsp(int peer_uid, QString peer_name, QString peer_nick, QString peer_icon, int peer_sex)
        : _uid(peer_uid), _name(peer_name), _nick(peer_nick), _icon(peer_icon), _sex(peer_sex) {}

    int _uid;
    QString _name;
    QString _nick;
    QString _icon;
    int _sex;
};

#endif // USERDATA_H
