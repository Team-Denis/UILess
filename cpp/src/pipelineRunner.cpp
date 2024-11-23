#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include <cstdio>

#include <nlohmann/json.hpp>

#include "commandHandler.hpp"
#include "pipelineRunner.hpp"

std::string runPipeline(const CommandPipeline& pipeline) {
    // Serialize the pipeline to JSON
    nlohmann::json json_output = pipeline.as_json();

    // Generate a unique temporary file name
    auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::string temp_json_file = "temp_pipeline_" + std::to_string(timestamp) + ".json";

    // Write the JSON to the temporary file
    {
        std::ofstream ofs(temp_json_file);
        if (!ofs) {
            throw std::runtime_error("Failed to open temporary file for writing");
        }
        ofs << json_output.dump(4);
        ofs.close();
    }

    // Define the full path to the Python script
    std::string python_script_path = "../../python/main.py";

    // Construct the command to invoke the Python script with the full path
    std::string python_command = "python \"" + python_script_path + "\" -f " + temp_json_file;

    // Open a pipe to read the output from the Python script
    FILE* pipe = popen(python_command.c_str(), "r");
    if (!pipe) {
        throw std::runtime_error("Failed to open pipe to Python script");
    }

    // Read the output from the Python script
    char buffer[128];
    std::string result;

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }

    // Close the pipe
    pclose(pipe);

    // Remove the temporary file
    std::remove(temp_json_file.c_str());

    // Return the result
    return result;
}