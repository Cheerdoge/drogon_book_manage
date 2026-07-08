#pragma once
#include <drogon/HttpFilter.h>

class AdminFilter : public drogon::HttpFilter<AdminFilter> {
public:
    void doFilter(const drogon::HttpRequestPtr& req,
                  drogon::FilterCallback&& fcb,
                  drogon::FilterChainCallback&& fccb) override;
};
