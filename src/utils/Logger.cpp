#include "Logger.hpp"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdio>   // rename
#include <sys/stat.h>
#include <unistd.h> // stat
#include <mutex>
#include <fstream>

std::ofstream Logger::logFile;
std::mutex Logger::logMutex;
std::string Logger::currentPath;
std::size_t Logger::maxFileSize = 10 * 1024 * 1024;

// 初始化
void Logger::init(const std::string& logPath, std::size_t maxSize) {
    currentPath = logPath;
    maxFileSize = maxSize;
    logFile.open(currentPath, std::ios::app);
    if (!logFile.is_open()) {
        std::cerr << "[" << getCurrentTime() << "] [WARN] "
                  << "Failed to open log file: " << currentPath << std::endl;
    }
}

void Logger::shutdown() {
    std::lock_guard<std::mutex> lock(logMutex);
    if (logFile.is_open()) logFile.close();
}

std::string Logger::getCurrentTime() {
    std::time_t now = std::time(nullptr);
    std::tm localTime{};
#ifdef _WIN32
    localtime_s(&localTime, &now);
#else
    localtime_r(&now, &localTime);
#endif
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

// 检查文件大小并轮转
void Logger::rotateIfNeeded() {
    if (!logFile.is_open()) return;

    logFile.flush();
    struct stat st;
    if (stat(currentPath.c_str(), &st) != 0) return;

    if (st.st_size >= maxFileSize) {
        logFile.close();

        // 生成新的文件名
        std::string backup = currentPath + ".1";
        std::remove(backup.c_str());      // 删除旧的轮转文件
        std::rename(currentPath.c_str(), backup.c_str());

        // 新建日志文件
        logFile.open(currentPath, std::ios::app);
    }
}

// 通用日志
void Logger::log(const std::string& level, const std::string& message) {
    std::string formatted = "[" + getCurrentTime() + "] [" + level + "] " + message;

    std::lock_guard<std::mutex> lock(logMutex);

    // 控制台输出
    if (level == "ERROR") std::cerr << formatted << std::endl;
    else std::cout << formatted << std::endl;

    // 文件输出
    if (logFile.is_open()) {
        rotateIfNeeded();        // 写之前检查轮转
        logFile << formatted << std::endl;
        logFile.flush();
    }
}

// info / warn / error / debug
void Logger::info(const std::string& msg)  { log("INFO", msg); }
void Logger::warn(const std::string& msg)  { log("WARN", msg); }
void Logger::error(const std::string& msg) { log("ERROR", msg); }
void Logger::debug(const std::string& msg) { log("DEBUG", msg); }