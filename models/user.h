#pragma once
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <string>
#include <optional>

namespace model {

struct User {
    int id = 0;
    std::string username;
    std::string password;
    std::string role;
    std::string createdAt;
    std::string updatedAt;

    static std::optional<User> findById(int id);
    static std::optional<User> findByUsername(const std::string& username);
    static std::vector<User> findAll();
    static bool create(const std::string& username, const std::string& password,
                       const std::string& role = "user");
    static bool updateRole(int id, const std::string& role);
    static bool updatePassword(int id, const std::string& password);

    Json::Value toJson() const;
};

} // namespace model
