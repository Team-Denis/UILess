#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>

#include <nlohmann/json.hpp>

#include "commandHandler.hpp"
#include "pipelineRunner.hpp"
#include "result.hpp"

// Helper Function: Serialize Pipeline to JSON
nlohmann::json serializePipelineToJson(const CommandPipeline& pipeline) {
    return pipeline.asJSON();
}

// Helper Function: Write JSON to a Temporary File
std::string writeJsonToTempFile(const nlohmann::json& json_output) {
    // Generate a unique temporary file name
    auto timestamp = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::string temp_json_file = "temp_pipeline_" + std::to_string(timestamp) + ".json";

    // Write the JSON to the temporary file
    std::ofstream ofs(temp_json_file);

    if (!ofs) {
        throw std::runtime_error("Failed to open temporary file for writing");
    }

    ofs << json_output.dump(4);
    ofs.close();

    return temp_json_file;
}

// Helper Function: Invoke Python Script
std::string invokePythonScript(const std::string& json_file_path, bool parallel) {
    // Define the full path to the Python script
    std::string python_script_path = "python/main.py";

    // Construct the command to invoke the Python script with the full path
    std::string python_command = "python \"" + python_script_path + "\" -f " + json_file_path;

    // Add the parallel flag if required
    if (parallel) {
        python_command += " -p";
    }

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

    return result;
}

// Helper Function: Process Python Output
std::vector<Result> processPythonOutput(const std::string& python_output) {
    std::vector<Result> results;

    try {
        // Parse the output as JSON
        nlohmann::json output_json = nlohmann::json::parse(python_output);

        if (output_json.is_array()) {
            // Multiple results (parallel execution)
            for (const auto& item : output_json) {
                Result res = item.get<Result>();
                results.push_back(res);
            }
        } else if (output_json.is_object()) {
            // Single result (sequential execution)
            Result res = output_json.get<Result>();
            results.push_back(res);
        }

    } catch (nlohmann::json::parse_error& e) {
        // Handle parsing errors by creating an error Result
        Result error_res;
        error_res.exit_code = -1;
        error_res.stdout_output = "";
        error_res.stderr_output = "JSON Parse Error: " + std::string(e.what());
        results.push_back(error_res);
    } catch (nlohmann::json::type_error& e) {
        // Handle type errors
        Result error_res;
        error_res.exit_code = -1;
        error_res.stdout_output = "";
        error_res.stderr_output = "JSON Type Error: " + std::string(e.what());
        results.push_back(error_res);
    } catch (...) {
        // Handle any other exceptions
        Result error_res;
        error_res.exit_code = -1;
        error_res.stdout_output = "";
        error_res.stderr_output = "Unknown Error during JSON parsing";
        results.push_back(error_res);
    }

    return results;
}

// Main Function: Run Pipeline
std::vector<Result> runPipeline(const CommandPipeline& pipeline) {
    // Serialize the pipeline to JSON
    nlohmann::json json_output = serializePipelineToJson(pipeline);

    // Write the JSON to a temporary file
    std::string temp_json_file = writeJsonToTempFile(json_output);

    // Get the bool more beautifully :whocares:
    bool parallel = pipeline.isParallel();

    // Invoke the Python script and capture the output
    std::string python_output = invokePythonScript(temp_json_file, parallel);

    // Remove the temporary file
    std::remove(temp_json_file.c_str());

    // Process the Python output and return it as a serialized JSON string
    std::vector<Result> results = processPythonOutput(python_output);

    return results;
}