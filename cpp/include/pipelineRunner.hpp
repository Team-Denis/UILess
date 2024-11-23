#pragma once

#include <string>

#include "commandHandler.hpp"
#include <result.hpp>

nlohmann::json      serializePipelineToJson (const CommandPipeline& pipeline);
std::string         writeJsonToTempFile     (const nlohmann::json& json_output);
std::string         invokePythonScript      (const std::string& json_file_path, bool parallel);
std::vector<Result> processPythonOutput     (const std::string& python_output);
std::vector<Result> runPipeline             (const CommandPipeline& pipeline);