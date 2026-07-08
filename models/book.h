#pragma once
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <string>
#include <optional>
#include <vector>

namespace model {

struct Book {
    int id = 0;
    std::string title;
    std::string author;
    std::string publisher;
    std::string isbn;
    std::string category;
    std::string status;
    std::string createdAt;
    std::string updatedAt;

    static std::optional<Book> findById(int id);
    static std::vector<Book> search(const std::string& title = "",
                                    const std::string& author = "",
                                    const std::string& category = "");
    static std::vector<Book> findAll();
    static int create(const std::string& title, const std::string& author,
                      const std::string& publisher, const std::string& isbn,
                      const std::string& category);
    static bool remove(int id);
    static bool update(int id, const std::string& title, const std::string& author,
                       const std::string& publisher, const std::string& isbn,
                       const std::string& category);
    static bool updateStatus(int id, const std::string& status);

    Json::Value toJson() const;
};

} // namespace model
