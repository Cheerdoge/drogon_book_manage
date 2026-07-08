#pragma once
#include <drogon/HttpController.h>

class BookController : public drogon::HttpController<BookController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(BookController::addBook, "/api/books", drogon::Post, "AuthFilter", "AdminFilter");
    ADD_METHOD_TO(BookController::deleteBook, "/api/books/{1}", drogon::Delete, "AuthFilter", "AdminFilter");
    ADD_METHOD_TO(BookController::updateBook, "/api/books/{1}", drogon::Put, "AuthFilter", "AdminFilter");
    ADD_METHOD_TO(BookController::getBooks, "/api/books", drogon::Get, "AuthFilter");
    ADD_METHOD_TO(BookController::getBookById, "/api/books/{1}", drogon::Get, "AuthFilter");
    METHOD_LIST_END

    void addBook(const drogon::HttpRequestPtr& req,
                 std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void deleteBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    int bookId);
    void updateBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    int bookId);
    void getBooks(const drogon::HttpRequestPtr& req,
                  std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getBookById(const drogon::HttpRequestPtr& req,
                     std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                     int bookId);
};
