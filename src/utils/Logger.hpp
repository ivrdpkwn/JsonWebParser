#pragma once
#include <string>
#include <fstream>
#include <mutex>


class Logger {
public:
    // 日志级别
    static void info(const std::string& message);
    static void error(const std::string& message);
    static void warn(const std::string& message);
    static void debug(const std::string& message);

    // 初始化 / 关闭
    static void init(const std::string& logPath , std::size_t maxSize);
    static void shutdown();

private:
    // 获取当前时间字符串
    static std::string getCurrentTime();
    
    // 日志文件流
    static std::ofstream logFile;
    static std::mutex logMutex;
    static std::string currentPath;
    static std::size_t maxFileSize;

    // 内部通用打印函数
    static void log(const std::string& level, const std::string& msg);
    static void rotateIfNeeded();
    
};
