#include "borrow_record.h"
#include <drogon/drogon.h>

namespace model {

static BorrowRecord rowToRecord(const drogon::orm::Row& row) {
    BorrowRecord r;
    r.id = row["Id"].as<int>();
    r.userId = row["UserId"].as<int>();
    r.bookId = row["BookId"].as<int>();
    r.borrowDate = row["BorrowDate"].as<std::string>();
    r.dueDate = row["DueDate"].isNull() ? "" : row["DueDate"].as<std::string>();
    r.returnDate = row["ReturnDate"].isNull() ? "" : row["ReturnDate"].as<std::string>();
    r.status = row["Status"].as<std::string>();
    return r;
}

std::vector<BorrowRecord> BorrowRecord::findByUser(int userId) {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync(
        "SELECT br.*, b.Title AS BookTitle, u.Username "
        "FROM BorrowRecords br "
        "JOIN Books b ON br.BookId = b.Id "
        "JOIN Users u ON br.UserId = u.Id "
        "WHERE br.UserId = $1 "
        "ORDER BY br.BorrowDate DESC",
        userId);
    std::vector<BorrowRecord> records;
    for (const auto& row : result) {
        BorrowRecord r = rowToRecord(row);
        r.bookTitle = row["BookTitle"].as<std::string>();
        r.username = row["Username"].as<std::string>();
        records.push_back(r);
    }
    return records;
}

std::vector<BorrowRecord> BorrowRecord::findAll() {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync(
        "SELECT br.*, b.Title AS BookTitle, u.Username "
        "FROM BorrowRecords br "
        "JOIN Books b ON br.BookId = b.Id "
        "JOIN Users u ON br.UserId = u.Id "
        "ORDER BY br.BorrowDate DESC");
    std::vector<BorrowRecord> records;
    for (const auto& row : result) {
        BorrowRecord r = rowToRecord(row);
        r.bookTitle = row["BookTitle"].as<std::string>();
        r.username = row["Username"].as<std::string>();
        records.push_back(r);
    }
    return records;
}

std::optional<BorrowRecord> BorrowRecord::findActiveByUserAndBook(int userId, int bookId) {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync(
        "SELECT * FROM BorrowRecords "
        "WHERE UserId = $1 AND BookId = $2 AND Status = 'active'",
        userId, bookId);
    if (result.empty()) return std::nullopt;
    return rowToRecord(result[0]);
}

bool BorrowRecord::borrow(int userId, int bookId, const std::string& dueDate) {
    auto db = drogon::app().getDbClient();
    try {
        db->execSqlSync(
            "INSERT INTO BorrowRecords (UserId, BookId, DueDate) VALUES ($1, $2, $3)",
            userId, bookId, dueDate);
        return true;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

bool BorrowRecord::returnBook(int recordId) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync(
            "UPDATE BorrowRecords SET Status='returned', ReturnDate=CURRENT_TIMESTAMP "
            "WHERE Id=$1 AND Status='active'",
            recordId);
        return result.affectedRows() > 0;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

Json::Value BorrowRecord::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["userId"] = userId;
    json["bookId"] = bookId;
    json["borrowDate"] = borrowDate;
    json["dueDate"] = dueDate;
    json["returnDate"] = returnDate;
    json["status"] = status;
    if (!bookTitle.empty()) json["bookTitle"] = bookTitle;
    if (!username.empty()) json["username"] = username;
    return json;
}

} // namespace model
