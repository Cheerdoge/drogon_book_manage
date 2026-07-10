#include "auth_filter.h"
#include "utils/jwt_util.h"
#include "utils/response.h"
#include <drogon/drogon.h>
#include <algorithm>
#include <cctype>

static std::string sanitizeToken(const std::string& raw) {
    std::string token = raw;
    while (!token.empty() && std::isspace(static_cast<unsigned char>(token.front()))) {
        token.erase(token.begin());
    }
    while (!token.empty() && std::isspace(static_cast<unsigned char>(token.back()))) {
        token.pop_back();
    }
    while (!token.empty() && (token.front() == '"' || token.front() == '\'')) {
        token.erase(token.begin());
    }
    while (!token.empty() && (token.back() == '"' || token.back() == '\'')) {
        token.pop_back();
    }
    token.erase(std::remove(token.begin(), token.end(), '\r'), token.end());
    token.erase(std::remove(token.begin(), token.end(), '\n'), token.end());
    token.erase(std::remove(token.begin(), token.end(), '\0'), token.end());
    return token;
}

void AuthFilter::doFilter(const drogon::HttpRequestPtr& req,
                           drogon::FilterCallback&& fcb,
                           drogon::FilterChainCallback&& fccb) {
    auto authHeader = req->getHeader("Authorization");

    if (authHeader.empty()) {
        fcb(utils::makeResponse(401, "未提供认证令牌"));
        return;
    }

    std::string token;
    if (authHeader.find("Bearer ") == 0) {
        token = authHeader.substr(7);
    } else if (authHeader.find("Bearer%20") == 0) {
        token = authHeader.substr(9);
    } else {
        LOG_WARN << "[AuthFilter] Authorization header does not start with 'Bearer '";
        fcb(utils::makeResponse(401, "未提供认证令牌"));
        return;
    }

    token = sanitizeToken(token);

    auto& config = drogon::app().getCustomConfig();
    std::string secret = config.get("jwt_secret", "default-secret").asString();

    std::string payload;
    if (!utils::verifyJWT(token, secret, payload)) {
        LOG_WARN << "[AuthFilter] JWT verification failed";
        fcb(utils::makeResponse(401, "令牌无效或已过期"));
        return;
    }

    Json::Value payloadJson;
    Json::Reader reader;
    if (!reader.parse(payload, payloadJson)) {
        LOG_ERROR << "[AuthFilter] JSON parse error: "
                  << reader.getFormattedErrorMessages();
        fcb(utils::makeResponse(401, "令牌解析失败"));
        return;
    }

    req->attributes()->insert("userId", payloadJson["userId"].asInt());
    req->attributes()->insert("username", payloadJson["username"].asString());
    req->attributes()->insert("role", payloadJson["role"].asString());

    fccb();
}
