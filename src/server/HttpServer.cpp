#include "HttpServer.hpp"
#include "../../third_party/httplib.h"
#include "../controller/ParseController.hpp"
#include "../utils/Logger.hpp"
#include "../utils/FileUtil.hpp"
#include "../middleware/LoggerMiddleware.hpp"
#include "../middleware/ErrorMiddleware.hpp"
#include "../config/config.hpp"
#include <iostream>


void HttpServer::startServer() {
    httplib::Server server;
    
    // 设置 httplib 内置线程池大小（关键）
    server.new_task_queue = [] {
        return new httplib::ThreadPool(Config::getThreadPoolSize());
    };

    // 静态文件目录
    server.set_mount_point("/", Config::getStaticPath());
    int maxRequestSize = Config::getMaxRequestSize();  // 最大请求体大小
    int port = Config::getPort();                      // 端口号
    

    // POST /api/parse 解析 JSON
    server.Post("/api/parse", [maxRequestSize](const httplib::Request& req, httplib::Response& res){
        LoggerMiddleware::logRequest(req);
        try {
            // 请求体过大，返回错误响应
            if (req.body.size() > maxRequestSize) {
                res.status = 413; // Payload Too Large
                res.set_content(ApiResponse::error("Request too large"), "application/json");
                LoggerMiddleware::logResponse(res);
                return; // 这里 return 只是结束 lambda，不返回值
            }

            // 解析并格式化 JSON
            std::string responseBody = ParseController::handleParseRequest(req.body);

            // 判断返回内容是否为 error
            nlohmann::ordered_json j = nlohmann::ordered_json::parse(responseBody);
            if (j.contains("status") && j["status"] == "error") {
                res.status = 400; // Bad Request
            } else {
                res.status = 200; // OK
            }

            // req.body 是 POST 请求体 
            res.set_content(responseBody, "application/json");

            LoggerMiddleware::logResponse(res);
        } 
        catch (const std::exception& e) {
            res.status = 500; // Internal Server Error
            res.set_content(ApiResponse::error("Internal Server Error: " + std::string(e.what())), "application/json");
            LoggerMiddleware::logResponse(res);
        }

    });
    
    Logger::info("Server started on port " + std::to_string(port) + "...");

    // 启动服务
    server.listen("0.0.0.0", port);

}
