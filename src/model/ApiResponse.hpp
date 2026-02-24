#pragma once 
#include <string>
#include "../../third_party/json.hpp" 

class ApiResponse {
public:
    // succes 
    static std::string success(const nlohmann::ordered_json& data);

    // error
    static std::string error(const std::string& message);

};