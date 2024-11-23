#include <iostream>
#include <thread>
#include <memory>

#include "commandHandler.hpp"
#include "pipelineRunner.hpp"

void executePipelineInThread(const CommandPipeline& pipeline) {
    try {
        std::string output = runPipeline(pipeline);
        // Process the output as needed
        std::cout << "Pipeline Output:\n" << output << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Exception in pipeline execution: " << e.what() << std::endl;
    }
}

int main() {
    // Create commands using the commandHandler classes
    auto start_cmd      = std::make_shared<StartCommand>    ("ls", StringList{}, 0);
    // auto middle_cmd1    = std::make_shared<MiddleCommand>   ("grep", StringList{".cpp"}, 0);
    // auto end_cmd        = std::make_shared<EndCommand>      ("FILEWRITE", StringList{"output.txt"}, 0);

    // Create a PipelineItem
    auto pipeline_item  = std::make_shared<PipelineItem>();
    pipeline_item->     set_start_command   (std::move(start_cmd));
    // pipeline_item->     add_middle_command  (std::move(middle_cmd1));
    // pipeline_item->     set_end_command     (std::move(end_cmd));

    // Create the CommandPipeline
    CommandPipeline pipeline;
    pipeline.add_pipeline_item(pipeline_item);

    // Start the thread and pass the pipeline
    std::thread pipeline_thread(executePipelineInThread, pipeline);

    // Wait for the thread to finish
    pipeline_thread.join();

    return 0;
}