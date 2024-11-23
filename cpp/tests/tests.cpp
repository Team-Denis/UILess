#include <iostream>
#include <memory>
#include <vector>

#include "commandHandler.hpp"
#include "pipelineRunner.hpp"

int main() {
    try {
        // Create the first PipelineItem: ls | grep .cpp | FILEWRITE output1.txt
        auto start_cmd1     = std::make_shared<StartCommand>    ("ls", StringList{}, 0);
        auto middle_cmd1    = std::make_shared<MiddleCommand>   ("grep", StringList{".txt"}, 0);
        // auto end_cmd1       = std::make_shared<EndCommand>      ("FILEWRITE", StringList{"output1.txt"}, 0);

        auto pipeline_item1 = std::make_shared<PipelineItem>();
        pipeline_item1->    set_start_command(start_cmd1);
        pipeline_item1->    add_middle_command(middle_cmd1);
        // pipeline_item1->    set_end_command(end_cmd1);

        // Create the second PipelineItem: ps | grep python | FILEWRITE output2.txt
        auto start_cmd2     = std::make_shared<StartCommand>    ("ps", StringList{}, 0);
        auto middle_cmd2    = std::make_shared<MiddleCommand>   ("grep", StringList{"python"}, 0);
        // auto end_cmd2       = std::make_shared<EndCommand>      ("FILEWRITE", StringList{"output2.txt"}, 0);

        auto pipeline_item2 = std::make_shared<PipelineItem>();
        pipeline_item2->    set_start_command(start_cmd2);
        pipeline_item2->    add_middle_command(middle_cmd2);
        // pipeline_item2->    set_end_command(end_cmd2);

        // Create the CommandPipeline and add both PipelineItems
        CommandPipeline pipeline;
        pipeline.add_pipeline_item(pipeline_item1);
        pipeline.add_pipeline_item(pipeline_item2);

        // Set the parallel flag
        pipeline.set_parallel(true); // Set to 'false' for sequential execution

        // Execute the pipeline with parallel flag set to true
        std::vector<Result> results = runPipeline(pipeline);

        // Output the results
        std::cout << "Pipeline Output:\n";

        for (size_t i = 0; i < results.size(); ++i) {
            const Result& res = results[i];
            std::cout << "Pipeline " << i + 1 << ":\n";
            std::cout << "  Exit Code: " << res.exit_code << "\n";
            std::cout << "  Stdout: " << res.stdout_output;
            std::cout << "  Stderr: " << res.stderr_output << "\n\n";
        }

        // Iterate over the CommandPipeline items
        std::cout << "Iterating over CommandPipeline items:\n";
        int item_number = 1;
        for (const auto& item : pipeline) {
            nlohmann::json item_json = item->as_json();
            std::cout << "Pipeline Item " << item_number++ << " JSON:\n" << item_json.dump(4) << "\n\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in pipeline execution: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}