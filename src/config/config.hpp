#pragma once
#include <string>

class Config {
public:
    static void init();                         // 初始化参数
    static void load(const std::string& path);  // 读取配置

    // 路径相关getters
    static const std::string& getExeDir();      // 获取当前可执行文件所在目录
    static const std::string& getConfigPath();  // 获取配置文件路径
    static const std::string& getStaticPath();  // 获取静态资源路径
    static const std::string& getLogPath();     // 获取日志文件路径
    
    static int getPort();                       // 获取端口
    static int getMaxRequestSize();       // 返回 max_request_size
    static int getMaxJsonDepth();         // 返回 max_json_depth
    static int getThreadPoolSize();       // 返回 ThreadPoolSize
    
private:

    static std::string exeDir;                  // 当前可执行文件所在目录
    static std::string configPath;              // 配置文件路径
    static std::string staticPath;              // 静态目录路径
    static std::string logPath;                 // 日志文件路径

    static int port;                            // 端口
    static int maxRequestSize;                  // 最大请求体大小
    static int maxJsonDepth;                    // 最大 JSON 嵌套深度
    static int threadPoolSize;                  // 线程池大小

    static std::string getExecutableDir();      // 获取当前可执行文件所在目录
    
};