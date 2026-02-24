#pragma once
#include <string>

class Config {
public:
    // 读取配置
    static void load(const std::string& path = "../../src/config/config.json");

    // 获取端口
    static int getPort();

    // 获取静态目录路径
    static std::string getStaticPath();

    // 返回 max_request_size
    static int getMaxRequestSize();   

    // 返回 max_json_depth
    static int getMaxJsonDepth();   
    
    // 返回 ThreadPoolSize
    static int getThreadPoolSize();

private:
    static int port;
    static std::string staticPath;
    static int maxRequestSize;    
    static int maxJsonDepth; 
    static int threadPoolSize;
    
};