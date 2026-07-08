#include "admin_filter.h"
#include "utils/response.h"

void AdminFilter::doFilter(const drogon::HttpRequestPtr& req,
                            drogon::FilterCallback&& fcb,
                            drogon::FilterChainCallback&& fccb) {
    auto role = req->attributes()->get<std::string>("role");
    if (role != "admin") {
        fcb(utils::makeResponse(403, "需要管理员权限"));
        return;
    }
    fccb();
}
