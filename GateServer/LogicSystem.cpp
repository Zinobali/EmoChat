#include "LogicSystem.h"
#include "HttpConnection.h"
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>
#include "global.h"

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

LogicSystem::LogicSystem() {
    RegisterGetHandler("/get_test", [](std::shared_ptr<HttpConnection> connection) {
        beast::ostream(connection->response_.body()) << "receive get_test req" << std::endl;
        int i = 0;
        for (auto& e : connection->get_params_) {
            i++;
            beast::ostream(connection->response_.body()) << "param" << i << " key is " << e.first;
            beast::ostream(connection->response_.body()) << ", " << " value is " << e.second << std::endl;
        }
        });

    RegisterPostHandler("/get_varifycode", [](std::shared_ptr<HttpConnection> connection) {
        auto body_str = beast::buffers_to_string(connection->request_.body().data());
        std::cout << "receive body is " << body_str << std::endl;
        connection->response_.set(http::field::content_type, "text/json");
        Json::Value root, src_root;
        Json::Reader reader;
        auto parse_success = reader.parse(body_str, src_root);

        // fail to parse json
        if (!parse_success) {
            std::cout << "Failed to parse JSON data!" << std::endl;
            root["error"] = static_cast<int>(ErrorCodes::Error_Json);
            auto json_str = root.toStyledString();
            beast::ostream(connection->response_.body()) << json_str;
            return true;
        }

        // success
        auto email = src_root["email"].asString();
        std::cout << "email is " << email << std::endl;
        root["error"] = static_cast<int>(ErrorCodes::Success);
        root["email"] = src_root["email"];
        auto json_str = root.toStyledString();
        beast::ostream(connection->response_.body()) << json_str;
        return true;
        });
}


