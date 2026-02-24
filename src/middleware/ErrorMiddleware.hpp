#pragma once
#include <functional>
#include <string>
#include "Logger.hpp"
#include "../model/ApiResponse.hpp"

class ErrorMiddleware {
public:
    // 捕获 controller 异常，返回标准 JSON
    static std::string handle(const std::function<std::string()>& controllerFunc);
};