#pragma once

#include "Singleton.h"
#include <map>
#include <functional>
#include <string>

class HttpConnection;

typedef std::function<void(std::shared_ptr<HttpConnection>)> HttpHandler;
class LogicSystem :public Singleton<LogicSystem>
{
    friend class Singleton<LogicSystem>;
public:
    ~LogicSystem();
    bool HandleGet(std::string url, std::shared_ptr<HttpConnection> connection);
    bool HandlePost(std::string url, std::shared_ptr<HttpConnection> connection);

private:
    void RegisterGetHandler(std::string url, HttpHandler handler);
    void RegisterPostHandler(std::string url, HttpHandler handler);
    void InitGetHandlers();
    void InitPostHandlers();

    LogicSystem();
    std::map<std::string, HttpHandler> get_handlers_;
    std::map<std::string, HttpHandler> post_handlers_;

};

