#pragma once

#include <string>
#include "commandHandler.hpp"

nlohmann::json serializePipelineToJson(const CommandPipeline& pipeline);
std::string writeJsonToTempFile(const nlohmann::json& json_output);
std::string invokePythonScript(const std::string& json_file_path, bool parallel);
std::string processPythonOutput(const std::string& python_output);

std::string runPipeline(const CommandPipeline& pipeline, bool parallel = false);