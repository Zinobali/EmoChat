#ifndef GLOBAL_H
#define GLOBAL_H

#include <QWidget>
#include <QStyle>
#include <functional>
#include <QString>
#include <QDebug>

extern std::function<void(QWidget*)> repolish;

extern QString gate_url_prefix;

enum class RequestId{
    ID_GET_VERIFY_CODE = 1001, //获取验证码
    ID_REG_USER = 1002, //注册用户
};

enum class Modules{
    REGISTERMOD = 0,
};

enum class ErrorCodes{
    SUCCESS = 0,
    ERR_JSON = 1, //Json解析失败
    ERR_NETWORK = 2,
};

enum class TipErr{
    TIP_SUCCESS = 0,
    TIP_EMAIL_ERR = 1,
    TIP_PWD_ERR = 2,
    TIP_CONFIRM_ERR = 3,
    TIP_PWD_CONFIRM = 4,
    TIP_VERIFY_ERR = 5,
    TIP_USER_ERR = 6
};

constexpr int toInt(const ErrorCodes& code) {
    return static_cast<int>(code);
}

constexpr int toInt(const TipErr& code) {
    return static_cast<int>(code);
}

#endif // GLOBAL_H
