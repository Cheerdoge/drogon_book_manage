#include <drogon/drogon.h>
#include <iostream>
#ifdef _WIN32
#include <windows.h>
#endif

#include "models/user.h"
#include "models/book.h"
#include "utils/bcrypt.h"

static void initDatabase() {
    auto db = drogon::app().getDbClient();

    db->execSqlSync(R"(
        CREATE TABLE IF NOT EXISTS Users (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            Username TEXT UNIQUE NOT NULL,
            Password TEXT NOT NULL,
            Role TEXT NOT NULL DEFAULT 'user',
            CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
            UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");

    db->execSqlSync(R"(
        CREATE TABLE IF NOT EXISTS Books (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            Title TEXT NOT NULL,
            Author TEXT,
            Publisher TEXT,
            Isbn TEXT UNIQUE,
            Category TEXT,
            Status TEXT NOT NULL DEFAULT 'available',
            CreatedAt DATETIME DEFAULT CURRENT_TIMESTAMP,
            UpdatedAt DATETIME DEFAULT CURRENT_TIMESTAMP
        )
    )");

    db->execSqlSync(R"(
        CREATE TABLE IF NOT EXISTS BorrowRecords (
            Id INTEGER PRIMARY KEY AUTOINCREMENT,
            UserId INTEGER NOT NULL,
            BookId INTEGER NOT NULL,
            BorrowDate DATETIME DEFAULT CURRENT_TIMESTAMP,
            DueDate DATETIME,
            ReturnDate DATETIME,
            Status TEXT NOT NULL DEFAULT 'active',
            FOREIGN KEY (UserId) REFERENCES Users(Id),
            FOREIGN KEY (BookId) REFERENCES Books(Id)
        )
    )");

    auto admin = model::User::findByUsername("admin");
    if (!admin.has_value()) {
        model::User::create("admin", utils::hashPassword("admin123"), "admin");
        LOG_INFO << "Seed: admin account created (admin / admin123)";
    }

    auto demoUser = model::User::findByUsername("user");
    if (!demoUser.has_value()) {
        model::User::create("user", utils::hashPassword("user123"), "user");
        LOG_INFO << "Seed: demo user created (user / user123)";
    }

    auto existingBooks = model::Book::findAll();
    if (existingBooks.empty()) {
        model::Book::create("三体", "刘慈欣", "重庆出版社", "9787536692930", "科幻");
        model::Book::create("活着", "余华", "作家出版社", "9787506365437", "文学");
        model::Book::create("百年孤独", "加西亚·马尔克斯", "南海出版公司", "9787544253994", "文学");
        model::Book::create("深入理解计算机系统", "Randal E. Bryant", "机械工业出版社", "9787115445353", "计算机");
        LOG_INFO << "Seed: 4 sample books created";
    }
}

int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif

    try {
        drogon::app().loadConfigFile("config.json");
    } catch (const std::exception& e) {
        std::cerr << "Failed to load config: " << e.what() << std::endl;
        return 1;
    }

    auto& customConfig = drogon::app().getCustomConfig();
    if (!customConfig.isMember("jwt_secret")) {
        LOG_ERROR << "config.json is missing jwt_secret";
        return 1;
    }

    drogon::app().registerBeginningAdvice([&]() { initDatabase(); });

    LOG_INFO << "Server starting on http://127.0.0.1:8080";
    drogon::app().run();
    return 0;
}
