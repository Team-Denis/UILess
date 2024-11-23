#include <iostream>
#include <vector>
#include <chrono>
#include <thread>
#include <cassert>

#include "commandHandler.hpp"
#include "cmdThread.hpp"
#include "result.hpp"

int main() {
    try {
        ThreadSafeCmdProcessor processor;
        processor.startThread(); // Start the worker thread before pushing tasks !!!!!!!!

        const int num_echo_tasks = 10;
        const int num_ls_tasks = 1;
        const int total_tasks = num_echo_tasks + num_ls_tasks;

        std::vector<std::string> expected_outputs;

        // Create and push echo "Hello World" tasks
        for (int i = 0; i < num_echo_tasks; ++i) {
            StartCommand start_cmd("echo", StringList{"Hello World"}, 0);
            PipelineItem pipeline_item;
            pipeline_item.setStartCommand(start_cmd);

            CommandPipeline pipeline;
            pipeline.addPipelineItem(pipeline_item);
            pipeline.setParallel(false);

            // Push the pipeline as a task
            processor.pushTask(pipeline);

            expected_outputs.emplace_back("Hello World\n");
        }

        // Create and push ls task
        for (int i = 0; i < num_ls_tasks; ++i) {
            StartCommand start_cmd("ls", StringList{}, 0);
            PipelineItem pipeline_item;
            pipeline_item.setStartCommand(start_cmd);

            CommandPipeline pipeline;
            pipeline.addPipelineItem(pipeline_item);
            pipeline.setParallel(false);

            processor.pushTask(pipeline);

            // For ls, we won't know the exact output, so we'll skip storing expected output
            // Instead, we'll just verify that the exit code is 0 and stdout is not empty
        }

        // Wait for all results with a timeout to prevent indefinite blocking !!!
        std::vector<Result> results;
        auto start_time = std::chrono::steady_clock::now();
        auto timeout = std::chrono::seconds(10);

        while (results.size() < static_cast<size_t>(total_tasks)) {
            if (processor.isResultAvailable()) {
                std::vector<Result> new_results = processor.popResults();
                results.insert(results.end(), new_results.begin(), new_results.end());
            }

            // Check for timeout
            if (std::chrono::steady_clock::now() - start_time > timeout) {
                std::cerr << "Timeout waiting for pipeline results.\n";
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        // Verify the number of results
        assert(results.size() == static_cast<size_t>(total_tasks));
        std::cout << "All " << total_tasks << " tasks have been processed.\n\n";

        int echo_success = 0;
        int ls_success = 0;

        // Iterate through the results and verify each one
        for (size_t i = 0; i < results.size(); ++i) {
            const Result& res = results[i];
            std::cout << "Pipeline " << i + 1 << " Output:\n";
            std::cout << "  Exit Code: " << res.exit_code << "\n";
            std::cout << "  Stdout: " << res.stdout_output;
            std::cout << "  Stderr: " << res.stderr_output << "\n\n";

            if (res.stdout_output.find("Hello World") != std::string::npos) {
                // This is an echo task
                assert(res.exit_code == 0); // Exit code should be 0 for success
                echo_success++;
            } else if (!res.stdout_output.empty() && res.exit_code == 0) {
                // Assuming that ls outputs something and has exit code 0
                ls_success++;
            } else {
                // Unexpected result
                std::cerr << "Unexpected result in pipeline " << i + 1 << ".\n";
                assert(false && "Unexpected pipeline result.");
            }
        }

        // Verify that all echo tasks succeeded
        assert(echo_success == num_echo_tasks);
        std::cout << "All echo tasks executed successfully.\n";

        // Verify that all ls tasks succeeded
        assert(ls_success == num_ls_tasks);
        std::cout << "All ls tasks executed successfully.\n";

        processor.stopThread();

        std::cout << "\nAll tests passed successfully.\n";

    } catch (const std::exception& e) {
        std::cerr << "Exception in threading tests: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}