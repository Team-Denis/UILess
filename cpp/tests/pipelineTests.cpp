#include <iostream>
#include <vector>

#include "commandHandler.hpp"
#include "pipelineRunner.hpp"

int main() {
    try {
        // First PipelineItem: ls | grep .txt | FILEWRITE output1.txt
        StartCommand    start_cmd1  ("ls", StringList{}, 0);
        MiddleCommand   middle_cmd1 ("grep", StringList{".txt"}, 0);
        // EndCommand      end_cmd1    ("FILEWRITE", StringList{"output1.txt"}, 0);

        PipelineItem    pipeline_item1;
        pipeline_item1.set_start_command    (start_cmd1);
        pipeline_item1.add_middle_command   (middle_cmd1);
        // pipeline_item1.set_end_command      (end_cmd1);

        // Second PipelineItem: ps | grep python | FILEWRITE output2.txt
        StartCommand    start_cmd2  ("ps", StringList{}, 0);
        MiddleCommand   middle_cmd2 ("grep", StringList{"python"}, 0);
        // EndCommand      end_cmd2    ("FILEWRITE", StringList{"output2.txt"}, 0);

        PipelineItem    pipeline_item2;
        pipeline_item2.set_start_command    (start_cmd2);
        pipeline_item2.add_middle_command   (middle_cmd2);
        //pipeline_item2.set_end_command      (end_cmd2);

        CommandPipeline pipeline;
        pipeline.add_pipeline_item(pipeline_item1);
        pipeline.add_pipeline_item(pipeline_item2);

        pipeline.set_parallel(true); // 'false' -> sequential execution
        // Execute the pipeline
        std::vector<Result> results = runPipeline(pipeline);

        std::cout << "Pipeline Output:\n";
        for (size_t i = 0; i < results.size(); ++i) {
            const Result& res = results[i];
            std::cout << "Pipeline " << i + 1 << ":\n";
            std::cout << "  Exit Code: " << res.exit_code << "\n";
            std::cout << "  Stdout: " << res.stdout_output;
            std::cout << "  Stderr: " << res.stderr_output << "\n\n";
        }

        // Iterate over the pipeline items
        std::cout << "Iterating over pipeline items:\n";
        int item_number = 1;
        for (const auto& item : pipeline) {
            nlohmann::json item_json = item.as_json();
            std::cout << "Pipeline Item " << item_number++ << " JSON:\n" << item_json.dump(4) << "\n\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "Exception in pipeline execution: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}