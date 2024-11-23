#pragma once

#include <string>
#include <nlohmann/json.hpp>

struct Result {
    int exit_code;
    std::string stdout_output;
    std::string stderr_output;
};

void to_json(nlohmann::json& j, const Result& res);
void from_json(const nlohmann::json& j, Result& res);