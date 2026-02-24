#include "server/HttpServer.hpp"
#include "config/config.hpp"
#include "utils/Logger.hpp"

int main() {
    // 初始化日志系统
    Logger::init(); // 5MB

    // 加载配置并启动服务器
    Config::load(); // 加载配置
    HttpServer::startServer(); 

    // 服务器关闭时清理资源
    Logger::shutdown();
    return 0;
}
