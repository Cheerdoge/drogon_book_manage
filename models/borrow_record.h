#pragma once
#include <drogon/orm/DbClient.h>
#include <json/json.h>
#include <string>
#include <optional>
#include <vector>

namespace model {

struct BorrowRecord {
    int id = 0;
    int userId = 0;
    int bookId = 0;
    std::string borrowDate;
    std::string dueDate;
    std::string returnDate;
    std::string status;

    std::string bookTitle;
    std::string username;

    static std::vector<BorrowRecord> findByUser(int userId);
    static std::vector<BorrowRecord> findAll();
    static std::optional<BorrowRecord> findActiveByUserAndBook(int userId, int bookId);
    static bool borrow(int userId, int bookId, const std::string& dueDate);
    static bool returnBook(int recordId);

    Json::Value toJson() const;
};

} // namespace model
