#include <iostream>
#include <vector>
#include <string>

#include "commandHandler.hpp"
#include "pipelineRunner.hpp"
#include "result.hpp"


int main() {
    try {
        CommandHandler handler;
        std::string input;

        std::cout << "Enter your command pipeline (use '-p' for parallel execution):\n> ";
        std::getline(std::cin, input);

        std::string errorMessage;

        if (!handler.parseInput(input, errorMessage)) {
            std::cerr << "Error: " << errorMessage << std::endl;
            return 1;
        }

        CommandPipeline pipeline = handler.getPipeline();
        std::vector<Result> results = runPipeline(pipeline);

        // Display results
        std::cout << "\nPipeline Execution Results:\n";

        for (size_t i = 0; i < results.size(); ++i) {
            const Result& res = results[i];
            std::cout << "Command " << i + 1 << ":\n";
            std::cout << "  Exit Code: " << res.exit_code << "\n";
            std::cout << "  Stdout: " << res.stdout_output << "\n";
            std::cout << "  Stderr: " << res.stderr_output << "\n\n";
        }

        // Iterate over the pipeline items and display their JSON representation
        std::cout << "Pipeline JSON Structure:\n";
        int item_number = 1;

        for (const auto& item : pipeline) {
            nlohmann::json item_json = item.asJSON();
            std::cout << "Pipeline Item " << item_number++ << " JSON:\n" << item_json.dump(4) << "\n\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in pipeline execution: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}