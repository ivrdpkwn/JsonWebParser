#include "FileUtil.hpp"
#include <fstream>
#include <sstream>
#include <sys/stat.h> // 用于 exists
#include <iostream>   // 可选，调试用


/* readFile
 * 打开文件
 * 读取内容到字符串
 * 关闭文件
 * 如果文件不存在 → 可以抛异常或返回空字符串 */

std::string FileUtil::readFile(const std::string& path) {
    // 1. 创建 std::ifstream RALL自动析构
    std::ifstream file(path, std::ios::in);

    // 2. 判断文件是否成功打开 Controller 应该捕获异常并返回 500
    if (!file.is_open()){
        throw std::runtime_error("FileUtil::readFile - Cannot open file: " + path);
    }

    // 3. 用 std::stringstream 把内容读进 string
    std::stringstream buffer;
    buffer << file.rdbuf();

    // 4. 返回 string
    return buffer.str();

}


/* writeFile
 * 打开失败返回 false
 * 成功写入返回 true */

bool FileUtil::writeFile(const std::string& path, const std::string& content) {
    // 1. 创建 std::ofstream
    std::ofstream file(path, std::ios::out | std::ios::trunc);  // ios::trunc 如果文件存在 → 清空后写入

    // 2. 判断是否打开成功
    if (!file.is_open()){
        return false;
    }

    // 3. 写入 content
    file << content;

    if (file.fail()){
        return false;
    }

    // 4. 返回 true/false
    return true;
}




/* exists（可选）
 * 判断文件是否存在
 * 返回 true/false */

bool FileUtil::exists(const std::string& path) {
    struct stat buffer;

    return (stat(path.c_str(), &buffer) == 0);
}