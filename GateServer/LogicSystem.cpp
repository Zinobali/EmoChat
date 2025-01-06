#include "LogicSystem.h"
#include "HttpConnection.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "global.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"
#include "MySQLMgr.h"
#include "StatusGrpcClient.h"

LogicSystem::~LogicSystem() {
    std::cout << "LogicSystem::~LogicSystem()" << std::endl;
}

bool LogicSystem::HandleGet(std::string url, std::shared_ptr<HttpConnection> connection) {
    if (get_handlers_.find(url) == get_handlers_.end()) {
        return false;
    }

    get_handlers_[url](connection);
    return true;
}

bool LogicSystem::HandlePost(std::string url, std::shared_ptr<HttpConnection> connection) {
    if (post_handlers_.find(url) == post_handlers_.end()) {
        return false;
    }

    post_handlers_[url](connection);
    return true;
}

void LogicSystem::RegisterGetHandler(std::string url, HttpHandler handler) {
    get_handlers_.insert({ url,handler });
}

void LogicSystem::RegisterPostHandler(std::string url, HttpHandler handler) {
    post_handlers_.insert({ url,handler });
}

void LogicSystem::InitGetHandlers() {
    // get测试请求
    RegisterGetHandler("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->response_.body()) << "receive get_test req" << std::endl;
        int i = 0;
        for (auto& e : connection->get_params_) {
            i++;
            beast::ostream(connection->response_.body()) << "param" << i << " key is " << e.first;
            beast::ostream(connection->response_.body()) << ", " << " value is " << e.second << std::endl;
        }
        });
}

void LogicSystem::InitPostHandlers() {
    // 获取验证码
    RegisterPostHandler("/get_verifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value root, src_root;
        Json::Reader reader;
        auto parse_success = reader.parse(body_str, src_root);

        // fail to parse json
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = toInt(ErrorCodes::Error_Json);
            auto json_str = root.toStyledString();
            beast::ostream(connection->response_.body()) << json_str;
            return true;
        }

        // success
        auto email = src_root["email"].asString();
        GetVerifyRsp rsp = VerifyGrpcClient::GetInstance()->GetVerifyCode(email);
        std::cout << "email is " << email << std::endl;
        root["error"] = toInt(ErrorCodes::Success);
        root["email"] = src_root["email"];
        auto json_str = root.toStyledString();
        beast::ostream(connection->response_.body()) << json_str;
        return true;
        });
    // 用户注册
    RegisterPostHandler("/user_register", [](std::shared_ptr< HttpConnection > connection) {
        auto body_str = beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value root, src_root;
        Json::Reader reader;
        auto parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = toInt(ErrorCodes::Error_Json);
            beast::ostream(connection->response_.body()) << root.toStyledString(); // send error code
            return true;
        }

        auto name = src_root["user"].asString();
        auto pwd = src_root["passwd"].asString();
        auto confirm = src_root["confirm"].asString();
        auto email = src_root["email"].asString();

        if (pwd != confirm) {
            std::cout << "password err " << std::endl;
            root["error"] = toInt(ErrorCodes::PasswdErr);
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->response_.body()) << jsonstr;
            return true;
        }

        // 从redis查询验证码
        std::string verify_code;
        bool b_get_verify = RedisMgr::GetInstance().Get(CODEPREFIX + src_root["email"].asString(), verify_code);
        if (!b_get_verify) {
            std::cout << " get verify code expired" << std::endl;
            root["error"] = toInt(ErrorCodes::VerifyExpired);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }
        // 验证码是否正确
        if (verify_code != src_root["verifycode"].asString()) {
            std::cout << " verify code error" << std::endl;
            root["error"] = toInt(ErrorCodes::VerifyCodeErr);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }
        // 用户是否已存在(好像没用)
        /*bool b_usr_exist = RedisMgr::GetInstance().ExistsKey(src_root["user"].asString());
        if (b_usr_exist) {
            std::cout << " user exist" << std::endl;
            root["error"] = toInt(ErrorCodes::UserExist);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }*/

        int uid = MySQLMgr::GetInstance()->RegUser(name, email, pwd);
        if (uid == 0 || uid == -1) {
            std::cout << " user or email exist" << std::endl;
            root["error"] = toInt(ErrorCodes::UserExist);
            std::string jsonstr = root.toStyledString();
            beast::ostream(connection->response_.body()) << jsonstr;
            return true;
        }

        std::cout << " user register success" << std::endl;
        root["error"] = 0;
        root["uid"] = uid;
        root["email"] = email;
        root["user"] = name;
        root["passwd"] = pwd;
        root["confirm"] = confirm;
        root["verifycode"] = src_root["verifycode"].asString();
        beast::ostream(connection->response_.body()) << root.toStyledString();
        return true;
        });

    // 重置密码
    RegisterPostHandler("/reset_pwd", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value root, src_root;
        Json::Reader reader;
        auto parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = toInt(ErrorCodes::Error_Json);
            beast::ostream(connection->response_.body()) << root.toStyledString(); // send error code
            return true;
        }

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();
        // 校验验证码
        std::string verify_code;
        bool b_verified = RedisMgr::GetInstance().Get(CODEPREFIX + email, verify_code);
        if (!b_verified) {
            std::cout << " get verify code expired" << std::endl;
            root["error"] = toInt(ErrorCodes::VerifyExpired);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }
        if (verify_code != src_root["verifycode"].asString()) {
            //std::cout << " verify in redis is: " << verify_code << std::endl;
            std::cout << " verify code error" << std::endl;
            root["error"] = toInt(ErrorCodes::VerifyCodeErr);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }
        // 检查邮箱是否存在
        bool email_valid = MySQLMgr::GetInstance()->EmailExist(email);
        if (!email_valid) {
            std::cout << " email not exist" << std::endl;
            root["error"] = toInt(ErrorCodes::EmailNotMatch);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }
        // 更新密码
        bool b_update = MySQLMgr::GetInstance()->UpdatePwd(email, pwd);
        if (!b_update) {
            std::cout << " update pwd failed" << std::endl;
            root["error"] = toInt(ErrorCodes::PasswdUpFailed);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }
        // 成功
        std::cout << "succeed to update password" << pwd << std::endl;
        root["error"] = 0;
        root["email"] = email;
        root["passwd"] = pwd;
        root["verifycode"] = verify_code;
        beast::ostream(connection->response_.body()) << root.toStyledString();
        return true;
        });

    // 用户登录
    RegisterPostHandler("/user_login", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value root, src_root;
        Json::Reader reader;
        auto parse_success = reader.parse(body_str, src_root);
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = toInt(ErrorCodes::Error_Json);
            beast::ostream(connection->response_.body()) << root.toStyledString(); // send error code
            return true;
        }

        auto email = src_root["email"].asString();
        auto pwd = src_root["passwd"].asString();
        UserInfo out_user;
        bool pwd_valid = MySQLMgr::GetInstance()->CheckPwd(email, pwd, out_user);
        if (!pwd_valid) {
            std::cout << "User pwd not match!" << std::endl;
            root["error"] = toInt(ErrorCodes::PasswdInvalid);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }

        // 从StatusServer获得连接
        auto reply = StatusGrpcClient::GetInstance()->GetChatServer(out_user.uid);
        if (reply.error()) {
            std::cout << " grpc get chat server failed, error is: " << reply.error() << std::endl;
            root["error"] = toInt(ErrorCodes::RPCFailed);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }

        std::cout << "succeed to load userinfo uid is: " << out_user.uid << std::endl;
        std::cout << "succeed to load userinfo name is: " << out_user.name << std::endl;
        root["error"] = 0;
        root["user"] = out_user.name;
        root["uid"] = out_user.uid;
        root["email"] = out_user.email;
        root["host"] = reply.host();
        root["port"] = reply.port();
        root["token"] = reply.token();
        beast::ostream(connection->response_.body()) << root.toStyledString();
        return true;
        });
}

LogicSystem::LogicSystem() {
    InitGetHandlers();
    InitPostHandlers();
}


