#include "ApiResponse.hpp"

// success 响应
std::string ApiResponse::success(const nlohmann::ordered_json& data) {
    nlohmann::ordered_json j;
    j["status"] = "success";
    j["data"] = data;
    j["message"] = "";
    return j.dump(4);
}

// error 响应
std::string ApiResponse::error(const std::string& message) {
    nlohmann::ordered_json j;
    j["status"] = "error";
    j["data"] = "";
    j["message"] = message;
    return j.dump();
}
