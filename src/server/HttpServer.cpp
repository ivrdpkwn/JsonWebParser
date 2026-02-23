#include "HttpServer.h"
#include "../../third_party/httplib.h"
#include "../controller/ParseController.h"
#include <iostream>

void HttpServer::startServer(int port) {
    httplib::Server server;

    // 静态文件目录
    server.set_mount_point("/", "static");

    // POST /api/parse 解析 JSON
    server.Post("/api/parse", [](const httplib::Request& req, httplib::Response& res){
        // req.body 是 POST 请求体
        std::string responseBody = ParseController::handleParseRequest(req.body);
        res.set_content(responseBody, "application/json");
        
    });
    

    std::cout << "Server started on port " << port << "..." <<std::endl; 

    // 启动服务
    server.listen("0.0.0.0", port);

}
