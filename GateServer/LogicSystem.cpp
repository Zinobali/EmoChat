#include "LogicSystem.h"
#include "HttpConnection.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "global.h"
#include "VerifyGrpcClient.h"
#include "RedisMgr.h"

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
        // 从redis获取验证码
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
        bool b_usr_exist = RedisMgr::GetInstance().ExistsKey(src_root["user"].asString());
        if (b_usr_exist) {
            std::cout << " user exist" << std::endl;
            root["error"] = toInt(ErrorCodes::UserExist);
            beast::ostream(connection->response_.body()) << root.toStyledString();
            return true;
        }

        std::cout << " user register success" << std::endl;
        root["error"] = 0;
        root["email"] = src_root["email"];
        root["user"] = src_root["user"].asString();
        root["passwd"] = src_root["passwd"].asString();
        root["confirm"] = src_root["confirm"].asString();
        root["verifycode"] = src_root["verifycode"].asString();
        beast::ostream(connection->response_.body()) << root.toStyledString();
        return true;
        });

}

LogicSystem::LogicSystem() {
    InitGetHandlers();
    InitPostHandlers();
}


