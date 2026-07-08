#include "user_controller.h"
#include "models/user.h"
#include "utils/response.h"
#include "utils/bcrypt.h"
#include "utils/jwt_util.h"
#include <drogon/drogon.h>

void UserController::registerUser(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(utils::makeResponse(400, "请求体不是有效的 JSON"));
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    if (username.empty() || password.empty()) {
        callback(utils::makeResponse(400, "用户名和密码不能为空"));
        return;
    }

    if (username.length() < 3 || password.length() < 6) {
        callback(utils::makeResponse(400, "用户名至少3个字符，密码至少6个字符"));
        return;
    }

    auto existing = model::User::findByUsername(username);
    if (existing.has_value()) {
        callback(utils::makeResponse(400, "用户名已存在"));
        return;
    }

    std::string hashedPwd = utils::hashPassword(password);
    if (model::User::create(username, hashedPwd)) {
        callback(utils::makeResponse(200, "注册成功"));
    } else {
        callback(utils::makeResponse(500, "注册失败"));
    }
}

void UserController::login(const drogon::HttpRequestPtr& req,
                            std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(utils::makeResponse(400, "请求体不是有效的 JSON"));
        return;
    }

    std::string username = (*json)["username"].asString();
    std::string password = (*json)["password"].asString();

    if (username.empty() || password.empty()) {
        callback(utils::makeResponse(400, "用户名和密码不能为空"));
        return;
    }

    auto user = model::User::findByUsername(username);
    if (!user.has_value()) {
        callback(utils::makeResponse(401, "用户名或密码错误"));
        return;
    }

    if (!utils::verifyPassword(password, user->password)) {
        callback(utils::makeResponse(401, "用户名或密码错误"));
        return;
    }

    Json::Value payload;
    payload["userId"] = user->id;
    payload["username"] = user->username;
    payload["role"] = user->role;
    payload["iat"] = static_cast<Json::Int64>(std::time(nullptr));
    payload["exp"] = static_cast<Json::Int64>(std::time(nullptr) + 86400 * 7);

    Json::FastWriter writer;
    std::string payloadStr = writer.write(payload);
    payloadStr.erase(std::remove(payloadStr.begin(), payloadStr.end(), '\n'), payloadStr.end());

    auto& config = drogon::app().getCustomConfig();
    std::string secret = config.get("jwt_secret", "default-secret").asString();
    std::string token = utils::generateJWT(secret, payloadStr);

    Json::Value data;
    data["token"] = token;
    data["user"] = user->toJson();
    callback(utils::makeResponse(200, "登录成功", data));
}

void UserController::getUsers(const drogon::HttpRequestPtr&,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto users = model::User::findAll();
    Json::Value data(Json::arrayValue);
    for (const auto& user : users) {
        data.append(user.toJson());
    }
    callback(utils::makeResponse(200, "查询成功", data));
}

void UserController::getUserById(const drogon::HttpRequestPtr& req,
                                  std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                  int userId) {
    auto currentId = req->attributes()->get<int>("userId");
    auto role = req->attributes()->get<std::string>("role");
    if (role != "admin" && currentId != userId) {
        callback(utils::makeResponse(403, "只能查看自己的信息"));
        return;
    }

    auto user = model::User::findById(userId);
    if (!user.has_value()) {
        callback(utils::makeResponse(404, "用户不存在"));
        return;
    }
    callback(utils::makeResponse(200, "查询成功", user->toJson()));
}

void UserController::updateUserRole(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                     int userId) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(utils::makeResponse(400, "请求体不是有效的 JSON"));
        return;
    }

    std::string role = (*json)["role"].asString();
    if (role != "admin" && role != "user") {
        callback(utils::makeResponse(400, "角色只能是 admin 或 user"));
        return;
    }

    auto user = model::User::findById(userId);
    if (!user.has_value()) {
        callback(utils::makeResponse(404, "用户不存在"));
        return;
    }

    if (model::User::updateRole(userId, role)) {
        callback(utils::makeResponse(200, "角色更新成功"));
    } else {
        callback(utils::makeResponse(500, "角色更新失败"));
    }
}
