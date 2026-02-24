#include "ParseController.hpp"
#include "../service/JsonService.hpp"
#include "../utils/FileUtil.hpp"
#include "../utils/Logger.hpp"
#include "../model/ApiResponse.hpp"
#include "../config/config.hpp"
#include <iostream>

// 处理 /api/parse POST请求，输入请求体，返回响应内容
std::string  ParseController::handleParseRequest(const std::string& requestBody) {
    try {
        Logger::info("Received parse request");

        // 请求体过大，返回错误响应
        if (requestBody.size() > static_cast<size_t>(Config::getMaxRequestSize())) {
            Logger::warn("Request too large: " + std::to_string(requestBody.size()));
            return ApiResponse::error("Request too large");
        }

        if (!JsonService::isValidJson(requestBody)) {
            Logger::error("JSON parse failed");
            return ApiResponse::error("JSON Parse Error");
        }

        // JSON 深度限制 
        Logger::info("Checking JSON depth, max allowed: " + std::to_string(Config::getMaxJsonDepth()));
        auto j = nlohmann::ordered_json::parse(requestBody); // 已验证有效 JSON
        int maxDepth = Config::getMaxJsonDepth();

        std::function<int(const nlohmann::ordered_json&, int)> calcDepth;
        calcDepth = [&](const nlohmann::ordered_json& node, int currentDepth) -> int {
            if (currentDepth > maxDepth) {
                Logger::warn("JSON depth exceeded at depth " + std::to_string(currentDepth));
                throw std::runtime_error("JSON too deep");
            }
            int depth = currentDepth;
            if (node.is_object()) {
                for (auto& el : node.items()) {
                    depth = std::max(depth, calcDepth(el.value(), currentDepth + 1));
                }
            } else if (node.is_array()) {
                for (auto& el : node) {
                    depth = std::max(depth, calcDepth(el, currentDepth + 1));
                }
            }
            return depth;
        };

        calcDepth(j, 1); // 从 1 层开始计数
        Logger::info("JSON depth check passed");

        // 调用 Service 层进行 JSON 格式化
        auto parsedJson = nlohmann::ordered_json::parse(requestBody);

        // 成功，返回标准 JSON 响应
        Logger::info("JSON parse successful");
        return ApiResponse::success(parsedJson); // 4 spaces for indentation
    }
    catch (const std::exception& e){
        Logger::error(std::string("Exception in handleParseRequest: ") + e.what());
        return ApiResponse::error("Internal Server Error: " + std::string(e.what()));
    }

}


// 调用工具类 Fileuti1   处理 GET / 返回网页内容 
std::string ParseController::handleIndexRequest() {
    try
    {
        // 读静态文件
        std::string indexPath = Config::getStaticPath() + "/index.html";
        Logger::info("Serving index file: " + indexPath);
        return FileUtil::readFile(indexPath);
    }
    catch(const std::exception& e)
    {
        // 文件不存在或读取失败 返回错误信息。
        Logger::error(std::string("Failed to read index.html: ") + e.what());
        return std::string("<h1>500 Internal Server Error</h1><p>Internal server error occurred.</p>");
    }
    
}

