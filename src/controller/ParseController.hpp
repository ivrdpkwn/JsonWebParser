#pragma once
#include <string>

class ParseController {
public:
    // 处理 /api/parse POST请求，输入请求体，返回响应内容
    static std::string handleParseRequest(const std::string& requestBody);

    // 处理 GET / 返回网页内容
    static std::string handleIndexRequest();
};