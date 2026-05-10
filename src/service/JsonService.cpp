#include "JsonService.hpp"
#include "../../third_party/json.hpp"
#include "../utils/Logger.hpp"
#include <iostream>

/* formatJson()
 * 先 parse
 * 然后用 dump(4) 生成缩进为4空格的格式化字符串
 * 如果出错 → 返回错误信息字符串*/

std::string JsonService::formatJson(const std::string& input) {
    try {
        // 解析 JSON
        auto parsed = nlohmann::ordered_json::parse(input);

        // 格式化输出（4空格缩进）
        return parsed.dump(4);
    }
    catch (const nlohmann::ordered_json::parse_error& e){
        return std::string("JSON Parse Error: ") + e.what();
    }
    catch (const std::exception& e){
        return std::string("Unexpected Error: ") + e.what();
    }
}


/* isValidJson()
 * 用 nlohmann::ordered_json::parse()
 * 用 try-catch 捕获异常
 * 如果抛异常 → false
 * 没抛 → true*/

bool JsonService::isValidJson(const std::string& input) {
    try {
        auto parsed = nlohmann::ordered_json::parse(input);
        return true;
    }
    catch (...) {
        return false;
    }
}