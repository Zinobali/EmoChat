#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <QStyle>
#include <functional>
#include <QString>
#include <QDebug>
#include <QUrl>
#include <QRandomGenerator>

static std::vector<QString> strs = {
    "慢品人间烟火色，闲观万事岁月长。",
    "追赶日出就算我献给自己的浪漫。",
    "生活不在别处，当下即是全部。",
    "永远热爱，永远期待，永远浪漫。",
    "向着光的方向，肆意生长。",
    "把烦心事丢掉，腾出地方装鲜花。"};

static std::vector<QString> heads = {
    ":/images/head_1.png",
    ":/images/head_2.png",
    ":/images/head_3.png",
    ":/images/head_4.png",
    ":/images/head_5.png",
    ":/images/head_6.png",
    ":/images/head_7.png",
    ":/images/head_8.png"};

static std::vector<QString> names = {
    "James",
    "John",
    "Robert",
    "Michael",
    "David",
    "Emma",
    "Olivia",
    "Ava",
    "Isabella",
    "Sophia"};

extern std::function<void(QWidget *)> repolish;

extern QString gate_url_prefix;

class Defer
{
public:
    explicit Defer(std::function<void()> &&func) : func_(std::move(func)) {}
    ~Defer() { func_(); }
    Defer(const Defer &) = delete;
    Defer &operator=(const Defer &) = delete;
    Defer(Defer &&other) = default;

private:
    std::function<void()> func_;
};

enum RequestId
{
    ID_GET_VERIFY_CODE = 1001,          // 获取验证码
    ID_REG_USER = 1002,                 // 注册用户
    ID_RESET_PWD = 1003,                // 重置密码
    ID_LOGIN_USER = 1004,               // 用户登录
    ID_CHAT_LOGIN = 1005,               // 登陆聊天服务器
    ID_CHAT_LOGIN_RSP = 1006,           // 登陆聊天服务器回包
    ID_SEARCH_USER_REQ = 1007,          // 用户搜索请求
    ID_SEARCH_USER_RSP = 1008,          // 搜索用户回包
    ID_ADD_FRIEND_REQ = 1009,           // 添加好友申请
    ID_ADD_FRIEND_RSP = 1010,           // 申请添加好友回复
    ID_NOTIFY_ADD_FRIEND_REQ = 1011,    // 通知用户添加好友申请
    ID_AUTH_FRIEND_REQ = 1013,          // 认证好友请求
    ID_AUTH_FRIEND_RSP = 1014,          // 认证好友回复
    ID_NOTIFY_AUTH_FRIEND_REQ = 1015,   // 通知用户认证好友申请
    ID_TEXT_CHAT_MSG_REQ = 1017,        // 文本聊天信息请求
    ID_TEXT_CHAT_MSG_RSP = 1018,        // 文本聊天信息回复
    ID_NOTIFY_TEXT_CHAT_MSG_REQ = 1019, // 通知用户文本聊天信息
    ID_HEARTBEAT_REQ = 1020,            // 心跳请求
    ID_HEARTBEAT_RSP = 1021,            // 心跳回复
    ID_UPLOAD_FILE_REQ = 1022,          // 上传文件请求
    ID_UPLOAD_FILE_RSP = 1023,          // 上传文件回复
    ID_DOWNLOAD_FILE_REQ = 1024,        // 下载文件请求
    ID_DOWNLOAD_FILE_RSP = 1025,        // 下载文件回复
};

enum Modules
{
    REGISTERMOD = 0,
    RESETMOD = 1,
    LOGINMOD = 2,
};

enum ErrorCodes
{
    SUCCESS = 0,
    ERR_JSON = 1, // Json解析失败
    ERR_NETWORK = 2,
};

enum TipErr
{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VERIFY_ERR = 5,
    TIP_USER_ERR = 6
};

constexpr int toInt(const RequestId &code)
{
    return static_cast<int>(code);
}

constexpr int toInt(const ErrorCodes &code)
{
    return static_cast<int>(code);
}

constexpr int toInt(const TipErr &code)
{
    return static_cast<int>(code);
}

#endif // GLOBAL_H
