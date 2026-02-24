#include "config.hpp"
#include "../../third_party/json.hpp" // nlohmann/json
#include "../utils/Logger.hpp"
#include <fstream>
#include <iostream>

using json = nlohmann::json;

int Config::port = 8080;                   // 默认值
std::string Config::staticPath = "../../static"; // 默认值
int Config::maxRequestSize = 1048576;      // 默认值 1 MB
int Config::maxJsonDepth = 20;             // 默认值
int Config::threadPoolSize = 4;            // 默认值

void Config::load(const std::string& path) {
    std::ifstream inFile(path);
    if (!inFile.is_open()) {
        std::cerr << "Warning: 配置文件 " << path << " 打开失败，使用默认配置\n";
        Logger::warn(std::string("Warning: 配置文件") + path + ", 打开失败，使用默认配置");
        return;
    }

    try {
        json j;
        inFile >> j;

        if (j.contains("server")) {
            auto server = j["server"];
            if (server.contains("port")) port = server["port"].get<int>();
            if (server.contains("max_request_size")) maxRequestSize = server["max_request_size"].get<int>();
            if (server.contains("max_json_depth")) maxJsonDepth = server["max_json_depth"].get<int>();
            if (server.contains("thread_pool_size")) threadPoolSize = server["thread_pool_size"].get<int>();
        }   

        if (j.contains("static_path")) {
            staticPath = j["static_path"].get<std::string>();
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: 解析配置文件失败: " << e.what() << ", 使用默认配置\n";
        Logger::error(std::string("解析配置文件失败: ") + e.what() + ", 使用默认配置");
    }
}

int Config::getPort() {
    return port;
}

std::string Config::getStaticPath() {
    return staticPath;
}

// 返回 max_request_size
int Config::getMaxRequestSize(){
    return maxRequestSize;
}  

// 返回 max_json_dept
int Config::getMaxJsonDepth(){
    return maxJsonDepth;
}  

// 返回 ThreadPoolSize
int Config::getThreadPoolSize(){
    return threadPoolSize;
}