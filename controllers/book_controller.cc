#include "book_controller.h"
#include "models/book.h"
#include "utils/response.h"
#include <drogon/drogon.h>

void BookController::addBook(const drogon::HttpRequestPtr& req,
                              std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(utils::makeResponse(400, "请求体不是有效的 JSON"));
        return;
    }

    std::string title = (*json)["title"].asString();
    std::string author = (*json)["author"].asString();
    std::string publisher = (*json)["publisher"].asString();
    std::string isbn = (*json)["isbn"].asString();
    std::string category = (*json)["category"].asString();

    if (title.empty()) {
        callback(utils::makeResponse(400, "书名不能为空"));
        return;
    }

    int id = model::Book::create(title, author, publisher, isbn, category);
    if (id > 0) {
        auto book = model::Book::findById(id);
        callback(utils::makeResponse(200, "添加成功", book->toJson()));
    } else {
        callback(utils::makeResponse(500, "添加失败，ISBN 可能重复"));
    }
}

void BookController::deleteBook(const drogon::HttpRequestPtr&,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                 int bookId) {
    if (model::Book::remove(bookId)) {
        callback(utils::makeResponse(200, "删除成功"));
    } else {
        callback(utils::makeResponse(404, "图书不存在"));
    }
}

void BookController::updateBook(const drogon::HttpRequestPtr& req,
                                 std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                 int bookId) {
    auto json = req->getJsonObject();
    if (!json) {
        callback(utils::makeResponse(400, "请求体不是有效的 JSON"));
        return;
    }

    auto book = model::Book::findById(bookId);
    if (!book.has_value()) {
        callback(utils::makeResponse(404, "图书不存在"));
        return;
    }

    std::string title = (*json)["title"].asString();
    std::string author = (*json)["author"].asString();
    std::string publisher = (*json)["publisher"].asString();
    std::string isbn = (*json)["isbn"].asString();
    std::string category = (*json)["category"].asString();

    if (title.empty()) {
        callback(utils::makeResponse(400, "书名不能为空"));
        return;
    }

    if (model::Book::update(bookId, title, author, publisher, isbn, category)) {
        auto updated = model::Book::findById(bookId);
        callback(utils::makeResponse(200, "更新成功", updated->toJson()));
    } else {
        callback(utils::makeResponse(500, "更新失败"));
    }
}

void BookController::getBooks(const drogon::HttpRequestPtr& req,
                               std::function<void(const drogon::HttpResponsePtr&)>&& callback) {
    std::string title = req->getParameter("title");
    std::string author = req->getParameter("author");
    std::string category = req->getParameter("category");

    std::vector<model::Book> books;
    if (title.empty() && author.empty() && category.empty()) {
        books = model::Book::findAll();
    } else {
        books = model::Book::search(title, author, category);
    }

    Json::Value data(Json::arrayValue);
    for (const auto& book : books) {
        data.append(book.toJson());
    }
    callback(utils::makeResponse(200, "查询成功", data));
}

void BookController::getBookById(const drogon::HttpRequestPtr&,
                                  std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                                  int bookId) {
    auto book = model::Book::findById(bookId);
    if (!book.has_value()) {
        callback(utils::makeResponse(404, "图书不存在"));
        return;
    }
    callback(utils::makeResponse(200, "查询成功", book->toJson()));
}
