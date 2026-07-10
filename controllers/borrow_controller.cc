#include "borrow_controller.h"
#include "models/book.h"
#include "models/borrow_record.h"
#include "utils/response.h"
#include <drogon/drogon.h>
#include <ctime>
#include <iomanip>
#include <sstream>

static std::string getFutureDate(int days) {
    auto now = std::time(nullptr);
    auto future = now + days * 86400;
    std::tm tm;
#ifdef _WIN32
    localtime_s(&tm, &future);
#else
    localtime_r(&future, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

void BorrowController::borrowBook(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(utils::makeResponse(400, "请求体不是有效的 JSON"));
        return;
    }

    int bookId = (*json)["bookId"].asInt();
    if (bookId <= 0) {
        callback(utils::makeResponse(400, "无效的图书 ID"));
        return;
    }

    auto book = model::Book::findById(bookId);
    if (!book.has_value()) {
        callback(utils::makeResponse(404, "图书不存在"));
        return;
    }

    if (book->status != "available") {
        callback(utils::makeResponse(400, "该图书已被借出"));
        return;
    }

    int userId = req->attributes()->get<int>("userId");

    auto active = model::BorrowRecord::findActiveByUserAndBook(userId, bookId);
    if (active.has_value()) {
        callback(utils::makeResponse(400, "您已借阅过该图书且未归还"));
        return;
    }

    std::string dueDate = getFutureDate(30);

    if (model::BorrowRecord::borrow(userId, bookId, dueDate)) {
        model::Book::updateStatus(bookId, "borrowed");
        LOG_INFO << "用户 [" << req->attributes()->get<std::string>("username")
                 << "] 借阅了《" << book->title << "》";
        callback(utils::makeResponse(200, "借阅成功"));
    } else {
        callback(utils::makeResponse(500, "借阅失败"));
    }
}

void BorrowController::returnBook(const drogon::HttpRequestPtr& req,
                                   std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                   int recordId) {
    auto db = drogon::app().getDbClient();
    auto result = db->execSqlSync(
        "SELECT * FROM BorrowRecords WHERE Id = $1 AND Status = 'active'", recordId);
    if (result.empty()) {
        callback(utils::makeResponse(404, "借阅记录不存在或已归还"));
        return;
    }

    int userId = req->attributes()->get<int>("userId");
    auto role = req->attributes()->get<std::string>("role");
    int recordUserId = result[0]["UserId"].as<int>();
    int bookId = result[0]["BookId"].as<int>();

    if (role != "admin" && userId != recordUserId) {
        callback(utils::makeResponse(403, "只能归还自己借阅的图书"));
        return;
    }

    if (model::BorrowRecord::returnBook(recordId)) {
        model::Book::updateStatus(bookId, "available");
        auto book = model::Book::findById(bookId);
        LOG_INFO << "用户 [" << req->attributes()->get<std::string>("username")
                 << "] 归还了《" << (book.has_value() ? book->title : "未知") << "》";
        callback(utils::makeResponse(200, "归还成功"));
    } else {
        callback(utils::makeResponse(500, "归还失败"));
    }
}

void BorrowController::getMyHistory(const drogon::HttpRequestPtr& req,
                                     std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    int userId = req->attributes()->get<int>("userId");
    auto records = model::BorrowRecord::findByUser(userId);
    Json::Value data(Json::arrayValue);
    for (const auto& r : records) {
        data.append(r.toJson());
    }
    callback(utils::makeResponse(200, "查询成功", data));
}

void BorrowController::getAllHistory(const drogon::HttpRequestPtr&,
                                      std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto records = model::BorrowRecord::findAll();
    Json::Value data(Json::arrayValue);
    for (const auto& r : records) {
        data.append(r.toJson());
    }
    callback(utils::makeResponse(200, "查询成功", data));
}
