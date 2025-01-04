#ifndef USERDATA_H
#define USERDATA_H

#include <QString>
#include <memory>
#include <QJsonArray>
#include <vector>
#include <QJsonObject>

class SearchInfo
{
public:
    SearchInfo(int uid, QString name, QString nick, QString desc, int sex, QString icon);
    int _uid;
    QString _name;
    QString _nick;
    QString _desc;
    int _sex;
    QString _icon;
};

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

#endif // USERDATA_H
