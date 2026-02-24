#pragma once
#include "Logger.hpp"
#include "../../third_party/httplib.h"

class LoggerMiddleware {
public:
    // 请求日志
    static void logRequest(const httplib::Request& req);

    // 响应日志
    static void logResponse(const httplib::Response& res);
};