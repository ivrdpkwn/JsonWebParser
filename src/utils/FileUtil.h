#pragma once
#include <string>

class FileUtil {
public:
    /* 读取文本文件内容
       用 static 是为了不用实例化类就可以调用 */
    static std::string readFile(const std::string& path);

    /* 写入文本文件（可选） */
    static bool writeFile(const std::string& path, const std::string& cosntent);

    /* 检查文件是否存在（可选） */
    static bool exists(const std::string& path);
};