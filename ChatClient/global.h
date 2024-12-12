#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <QStyle>
#include <functional>
#include <QString>

extern std::function<void(QWidget*)> repolish;

extern QString gate_url_prefix;

enum class RequestId{
    ID_GET_VARIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum class ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};

enum class Modules{
    REGISTERMOD = 0,
};

#endif // GLOBAL_H
