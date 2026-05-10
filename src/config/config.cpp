#include "config.hpp"
#include "../../third_party/json.hpp" // nlohmann/json
#include "../utils/Logger.hpp"
#include <fstream>
#include <iostream>
#include <unistd.h>    
#include <limits.h>      

using json = nlohmann::json;

// ===== static 成员定义 =====
std::string Config::exeDir;
std::string Config::configPath;
std::string Config::staticPath;
std::string Config::logPath;

int Config::port;
int Config::maxRequestSize;
int Config::maxJsonDepth;
int Config::threadPoolSize;

// init初始化（路径相关）
void Config::init() {
    exeDir     = getExecutableDir();             // 获取当前可执行文件所在目录
    configPath = exeDir + "/../../src/config/config.json";   // 获取配置文件路径             // 默认值
    staticPath = exeDir + "/../../static";             // 静态资源路径
    logPath    = exeDir + "/../../logs/server.log";    // 日志文件路径

    // 明确默认值
    port = 8080;
    maxRequestSize = 1048576;
    maxJsonDepth = 20;
    threadPoolSize = 4;
}

// 读取配置
void Config::load(const std::string& path) {
    std::ifstream inFile(path);

    // 如果配置文件打开失败，记录警告日志并使用默认配置
    if (!inFile.is_open()) {
        std::cerr << "Warning: 配置文件 " << path << " 打开失败，使用默认配置\n";
        Logger::warn(std::string("Warning: 配置文件") + path + ", 打开失败，使用默认配置");
        return;
    }

    // 解析 JSON 配置文件
    try {
        json j;
        inFile >> j;

        // 解析配置项
        if (j.contains("server")) {
            auto server = j["server"];
            if (server.contains("port")) port = server["port"].get<int>();
            if (server.contains("max_request_size")) maxRequestSize = server["max_request_size"].get<int>();
            if (server.contains("max_json_depth")) maxJsonDepth = server["max_json_depth"].get<int>();
            if (server.contains("thread_pool_size")) threadPoolSize = server["thread_pool_size"].get<int>();
        }   

        if (j.contains("static_path")) {
            staticPath = exeDir + "/../../" + j["static_path"].get<std::string>();
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: 解析配置文件失败: " << e.what() << ", 使用默认配置\n";
        Logger::error(std::string("解析配置文件失败: ") + e.what() + ", 使用默认配置");
    }
}

// 获取配置项
int Config::getPort() { return port; }                            // 获取端口

const std::string& Config::getExeDir() { return exeDir; }         // 获取当前可执行文件所在目录
const std::string& Config::getStaticPath() { return staticPath; } // 获取静态资源路径
const std::string& Config::getConfigPath() { return configPath; } // 获取配置文件路径
const std::string& Config::getLogPath() { return logPath; }       // 获取日志文件路径

int Config::getMaxRequestSize(){ return maxRequestSize; }   // 获取最大请求体大小
int Config::getMaxJsonDepth(){ return maxJsonDepth; }       // 获取最大 JSON 嵌套深度
int Config::getThreadPoolSize(){ return threadPoolSize; }   // 获取线程池大小

// 获取当前可执行文件所在目录
std::string Config::getExecutableDir() {
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);  // 获取当前可执行文件的路径
    if (len == -1) { return ""; }
    buf[len] = '\0';                                        // 确保字符串以 null 结尾

    std::string fullPath(buf);
    return fullPath.substr(0, fullPath.find_last_of('/'));
}