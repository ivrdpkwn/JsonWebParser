#include "ParseController.h"
#include "../service/JsonService.h"
#include "../utils/FileUtil.h"
#include <iostream>

// 处理 /api/parse POST请求，输入请求体，返回响应内容
std::string  ParseController::handleParseRequest(const std::string& requestBody) {
    try {
        // 调用 Service 层进行 JSON 格式化
        std::string formattedJson = JsonService::formatJson(requestBody);

        // 检查是不是解析出错（formatJson出错会返回 “Parse Error” 的字符串）
        if (formattedJson.find("Parse Error") != std::string::npos ||
            formattedJson.find("Unexpected Error") != std::string::npos) {
            return R"({"status":"error","message":")"+ formattedJson + "\"}";
        }

        // 成功，返回标准 JSON 响应
        return R"({"status":"success","data":)" + formattedJson +"}";
    }
    catch (const std::exception& e){
        return R"({"status":"error000","message":")" + std::string(e.what()) + "\"}";
    }

}


// 调用工具类 Fileuti1   处理 GET / 返回网页内容 
std::string ParseController::handleIndexRequest() {
    try
    {
        // 读静态文件
        return FileUtil::readFile("static/index.html");
    }
    catch(const std::exception& e)
    {
        // 文件不存在或读取失败 返回错误信息。
        return std::string("<h1>500 Internal Server Error</h1><p>") + e.what() + "</p>";
    }
    
}

