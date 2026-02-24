#include "ErrorMiddleware.hpp"

std::string ErrorMiddleware::handle(const std::function<std::string()>& controllerFunc) {
    try {
        // 正常调用 controller
        return controllerFunc();
    } catch (const std::exception& e) {
        Logger::error(std::string("Controller exception: ") + e.what());
        return ApiResponse::error(e.what());
    } catch (...) {
        Logger::error("Controller unknown exception");
        return ApiResponse::error("未知错误");
    }
}