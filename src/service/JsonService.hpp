#pragma once
#include <string>

class JsonService{
public:
    // 解析并格式化 JSON
    static std::string formatJson(const std::string& input);

    // 检查 JSON 是否合法
    static bool isValidJson(const std::string& input);
    
};