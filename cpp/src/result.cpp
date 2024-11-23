#include "result.hpp"

// Serialization: Convert Result to JSON
void to_json(nlohmann::json& j, const Result& res) {
    j = nlohmann::json{
            {"exit_code", res.exit_code},
            {"stdout", res.stdout_output},
            {"stderr", res.stderr_output}
    };
}

// Deserialization: Convert JSON to Result
void from_json(const nlohmann::json& j, Result& res) {
    j.at("exit_code").get_to(res.exit_code);
    j.at("stdout").get_to(res.stdout_output);
    j.at("stderr").get_to(res.stderr_output);
}