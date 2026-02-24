#include "LoggerMiddleware.hpp"

void LoggerMiddleware::logRequest(const httplib::Request& req) {
    Logger::info("Incoming request: " + req.method + " " + req.path);
    if (!req.body.empty()) {
        Logger::info("Request body: " + req.body);
    }
}

void LoggerMiddleware::logResponse(const httplib::Response& res) {
    Logger::info("Response status: " + std::to_string(res.status));
    if (!res.body.empty()) {
        Logger::info("Response body: " + res.body);
    }
}