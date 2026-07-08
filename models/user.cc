#include "user.h"
#include <drogon/drogon.h>

namespace model {

std::optional<User> User::findById(int id) {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync("SELECT * FROM Users WHERE Id = $1", id);
    if (result.empty()) return std::nullopt;
    User user;
    user.id = result[0]["Id"].as<int>();
    user.username = result[0]["Username"].as<std::string>();
    user.password = result[0]["Password"].as<std::string>();
    user.role = result[0]["Role"].as<std::string>();
    user.createdAt = result[0]["CreatedAt"].as<std::string>();
    user.updatedAt = result[0]["UpdatedAt"].as<std::string>();
    return user;
}

std::optional<User> User::findByUsername(const std::string& username) {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync("SELECT * FROM Users WHERE Username = $1", username);
    if (result.empty()) return std::nullopt;
    User user;
    user.id = result[0]["Id"].as<int>();
    user.username = result[0]["Username"].as<std::string>();
    user.password = result[0]["Password"].as<std::string>();
    user.role = result[0]["Role"].as<std::string>();
    user.createdAt = result[0]["CreatedAt"].as<std::string>();
    user.updatedAt = result[0]["UpdatedAt"].as<std::string>();
    return user;
}

std::vector<User> User::findAll() {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync("SELECT * FROM Users ORDER BY Id");
    std::vector<User> users;
    for (const auto& row : result) {
        User user;
        user.id = row["Id"].as<int>();
        user.username = row["Username"].as<std::string>();
        user.password = row["Password"].as<std::string>();
        user.role = row["Role"].as<std::string>();
        user.createdAt = row["CreatedAt"].as<std::string>();
        user.updatedAt = row["UpdatedAt"].as<std::string>();
        users.push_back(user);
    }
    return users;
}

bool User::create(const std::string& username, const std::string& password,
                  const std::string& role) {
    auto db = drogon::app().getDbClient();
    try {
        db->execSqlSync(
            "INSERT INTO Users (Username, Password, Role) VALUES ($1, $2, $3)",
            username, password, role);
        return true;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

bool User::updateRole(int id, const std::string& role) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync(
            "UPDATE Users SET Role = $1, UpdatedAt = CURRENT_TIMESTAMP WHERE Id = $2",
            role, id);
        return result.affectedRows() > 0;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

bool User::updatePassword(int id, const std::string& password) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync(
            "UPDATE Users SET Password = $1, UpdatedAt = CURRENT_TIMESTAMP WHERE Id = $2",
            password, id);
        return result.affectedRows() > 0;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

Json::Value User::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["username"] = username;
    json["role"] = role;
    json["createdAt"] = createdAt;
    json["updatedAt"] = updatedAt;
    return json;
}

} // namespace model
