#pragma once
#include <functional>

constexpr char CODEPREFIX[] = "code_";

class Defer {
public:
    explicit Defer(std::function<void()>&& func) :func_(std::move(func)) {}
    ~Defer() { func_(); }
    Defer(const Defer&) = delete;
    Defer& operator=(const Defer&) = delete;
    Defer(Defer&& other) = default;

private:
    std::function<void()> func_;
};

enum class ErrorCodes {
    Success = 0,
    Error_Json = 1001,  //Json解析错误
    RPCFailed = 1002,  //RPC请求错误
    VerifyExpired = 1003, //验证码过期
    VerifyCodeErr = 1004, //验证码错误
    UserExist = 1005,       //用户已经存在
    PasswdErr = 1006,    //密码错误
    EmailNotMatch = 1007,  //邮箱不匹配
    PasswdUpFailed = 1008,  //更新密码失败
    PasswdInvalid = 1009,   //密码更新失败
    TokenInvalid = 1010,   //Token失效
    UidInvalid = 1011,  //uid无效
};

constexpr int toInt(const ErrorCodes& code) {
    return static_cast<int>(code);
}


