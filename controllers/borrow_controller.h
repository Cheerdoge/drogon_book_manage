#pragma once
#include <drogon/HttpController.h>

class BorrowController : public drogon::HttpController<BorrowController> {
public:
    METHOD_LIST_BEGIN
    ADD_METHOD_TO(BorrowController::borrowBook, "/api/borrow", drogon::Post, "AuthFilter");
    ADD_METHOD_TO(BorrowController::returnBook, "/api/borrow/{1}/return", drogon::Put, "AuthFilter");
    ADD_METHOD_TO(BorrowController::getMyHistory, "/api/borrow/history", drogon::Get, "AuthFilter");
    ADD_METHOD_TO(BorrowController::getAllHistory, "/api/borrow/history/all",
                  drogon::Get, "AuthFilter", "AdminFilter");
    METHOD_LIST_END

    void borrowBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void returnBook(const drogon::HttpRequestPtr& req,
                    std::function<void(const drogon::HttpResponsePtr&)>&& callback,
                    int recordId);
    void getMyHistory(const drogon::HttpRequestPtr& req,
                      std::function<void(const drogon::HttpResponsePtr&)>&& callback);
    void getAllHistory(const drogon::HttpRequestPtr& req,
                       std::function<void(const drogon::HttpResponsePtr&)>&& callback);
};
