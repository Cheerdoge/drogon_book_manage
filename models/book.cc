#include "book.h"
#include <drogon/drogon.h>
#include <sstream>

namespace model {

static Book rowToBook(const drogon::orm::Row& row) {
    Book book;
    book.id = row["Id"].as<int>();
    book.title = row["Title"].as<std::string>();
    book.author = row["Author"].isNull() ? "" : row["Author"].as<std::string>();
    book.publisher = row["Publisher"].isNull() ? "" : row["Publisher"].as<std::string>();
    book.isbn = row["Isbn"].isNull() ? "" : row["Isbn"].as<std::string>();
    book.category = row["Category"].isNull() ? "" : row["Category"].as<std::string>();
    book.status = row["Status"].as<std::string>();
    book.createdAt = row["CreatedAt"].as<std::string>();
    book.updatedAt = row["UpdatedAt"].as<std::string>();
    return book;
}

std::optional<Book> Book::findById(int id) {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync("SELECT * FROM Books WHERE Id = $1", id);
    if (result.empty()) return std::nullopt;
    return rowToBook(result[0]);
}

std::vector<Book> Book::search(const std::string& title,
                                const std::string& author,
                                const std::string& category) {
    auto db = drogon::app().getDbClient();
    std::ostringstream sql;
    sql << "SELECT * FROM Books WHERE 1=1";
    std::vector<std::string> params;
    if (!title.empty()) {
        sql << " AND Title LIKE $1";
        params.push_back("%" + title + "%");
    }
    if (!author.empty()) {
        sql << " AND Author LIKE $" << (params.size() + 1);
        params.push_back("%" + author + "%");
    }
    if (!category.empty()) {
        sql << " AND Category LIKE $" << (params.size() + 1);
        params.push_back("%" + category + "%");
    }
    sql << " ORDER BY Id";

    auto result = db->execSqlSync(sql.str(), params[0],
                                  params.size() > 1 ? params[1] : "",
                                  params.size() > 2 ? params[2] : "");
    std::vector<Book> books;
    for (const auto& row : result) {
        books.push_back(rowToBook(row));
    }
    return books;
}

std::vector<Book> Book::findAll() {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync("SELECT * FROM Books ORDER BY Id");
    std::vector<Book> books;
    for (const auto& row : result) {
        books.push_back(rowToBook(row));
    }
    return books;
}

int Book::create(const std::string& title, const std::string& author,
                  const std::string& publisher, const std::string& isbn,
                  const std::string& category) {
    auto db = drogon::app().getDbClient();
    try {
        db->execSqlSync(
            "INSERT INTO Books (Title, Author, Publisher, Isbn, Category) "
            "VALUES ($1, $2, $3, $4, $5)",
            title, author, publisher, isbn, category);
        return static_cast<int>(db->execSqlSync("SELECT last_insert_rowid()")[0][0].as<int64_t>());
    } catch (const drogon::orm::DrogonDbException&) {
        return -1;
    }
}

bool Book::remove(int id) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync("DELETE FROM Books WHERE Id = $1", id);
        return result.affectedRows() > 0;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

bool Book::update(int id, const std::string& title, const std::string& author,
                   const std::string& publisher, const std::string& isbn,
                   const std::string& category) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync(
            "UPDATE Books SET Title=$1, Author=$2, Publisher=$3, Isbn=$4, "
            "Category=$5, UpdatedAt=CURRENT_TIMESTAMP WHERE Id=$6",
            title, author, publisher, isbn, category, id);
        return result.affectedRows() > 0;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

bool Book::updateStatus(int id, const std::string& status) {
    auto db = drogon::app().getDbClient();
    try {
        auto result = db->execSqlSync(
            "UPDATE Books SET Status=$1, UpdatedAt=CURRENT_TIMESTAMP WHERE Id=$2",
            status, id);
        return result.affectedRows() > 0;
    } catch (const drogon::orm::DrogonDbException&) {
        return false;
    }
}

Json::Value Book::toJson() const {
    Json::Value json;
    json["id"] = id;
    json["title"] = title;
    json["author"] = author;
    json["publisher"] = publisher;
    json["isbn"] = isbn;
    json["category"] = category;
    json["status"] = status;
    json["createdAt"] = createdAt;
    json["updatedAt"] = updatedAt;
    return json;
}

} // namespace model
