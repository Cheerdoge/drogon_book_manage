#pragma once
#include <drogon/HttpResponse.h>
#include <json/json.h>

namespace utils {

inline drogon::HttpResponsePtr makeResponse(int code, const std::string& message,
                                            const Json::Value& data = Json::Value()) {
    Json::Value resp;
    resp["code"] = code;
    resp["message"] = message;
    resp["data"] = data;
    auto httpResp = drogon::HttpResponse::newHttpJsonResponse(resp);
    if (code == 200) {
        httpResp->setStatusCode(drogon::k200OK);
    } else if (code == 400) {
        httpResp->setStatusCode(drogon::k400BadRequest);
    } else if (code == 401) {
        httpResp->setStatusCode(drogon::k401Unauthorized);
    } else if (code == 403) {
        httpResp->setStatusCode(drogon::k403Forbidden);
    } else if (code == 404) {
        httpResp->setStatusCode(drogon::k404NotFound);
    } else {
        httpResp->setStatusCode(drogon::k500InternalServerError);
    }
    return httpResp;
}

} // namespace utils
