#pragma once
#include <drogon/HttpController.h>

class UserController : public drogon::HttpController<UserController> {
public:
    METHOD_LIST_BEGIN
    // 公开接口
    ADD_METHOD_TO(UserController::registerUser, "/api/users/register", drogon::Post);
    ADD_METHOD_TO(UserController::login, "/api/users/login", drogon::Post);
    // 需要认证
    ADD_METHOD_TO(UserController::getUsers, "/api/users", drogon::Get, "AuthFilter", "AdminFilter");
    ADD_METHOD_TO(UserController::getUserById, "/api/users/{1}", drogon::Get, "AuthFilter");
    ADD_METHOD_TO(UserController::updateUserRole, "/api/users/{1}/role", drogon::Put, "AuthFilter", "AdminFilter");
    METHOD_LIST_END

    void registerUser(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void login(const drogon::HttpRequestPtr& req,
               std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getUsers(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getUserById(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                     int userId);
    void updateUserRole(const drogon::HttpRequestPtr& req,
                        std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                        int userId);
};
