#include <colors.hpp>
#include <raylib.h>
#include <commandHandler.hpp>
#include <unordered_map>
#include <vector>
#include <stdlib.h>
#include <iostream>
#include <string>

#include "cmdThread.hpp"
#include "imGui.hpp"

constexpr int START_WIDTH = 1280;
constexpr int START_HEIGHT = 800;

int main(int argc, char **argv) {
    // Prevents compiler from yapping
    (void)argc;
    (void)argv;

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(START_WIDTH, START_HEIGHT, "UILess");
    SetWindowMinSize(START_WIDTH * 2 / 3, START_HEIGHT * 2 / 3);

    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    SetTargetFPS(60);

    // Shader shader = LoadShader(nullptr, TextFormat("shaders/blur.glsl", 330));
    std::unordered_map<std::string,    std::pair<CommandType,                CommandArgType>> type_info;
    type_info.emplace("ls",         std::pair{CommandType::Start,   CommandArgType::None});
    type_info.emplace("cat",        std::pair{CommandType::Start,   CommandArgType::Filepath});
    type_info.emplace("grep",       std::pair{CommandType::Middle,  CommandArgType::Text});

    CommandPipeline pipeline;

    // Utility textures
    ImGui::loadTexture("run",  "assets/run.png");

    // Command textures
    ImGui::loadTexture("ls",   "assets/mag.png");
    ImGui::loadTexture("cat",  "assets/cat.png");
    ImGui::loadTexture("grep", "assets/grapes.png");

    ThreadSafeCmdProcessor processor;
    processor.startThread(); // Start the worker thread
    // Variables to handle results
    std::vector<Result> pending_results;
    std::vector<ImGui::OutputResult> output_results;

    PipelineItem item;

    // int resolutionLoc = GetShaderLocation(shader, "resolution");

    while (!WindowShouldClose()) {
        if (processor.isResultAvailable()) {
            std::vector<Result> new_results = processor.popResults();
            pending_results.insert(pending_results.end(), new_results.begin(), new_results.end());

            // Create output result struct
            for (const auto &result: new_results) {
                ImGui::OutputResult output_result;
                output_result.result = result;
                time_t t = time(nullptr);
                output_result.datetime = *localtime(&t);
                output_results.push_back(output_result);

                if (output_results.size() > max_output_result) {
                    output_results.erase(output_results.begin());
                }
            }

            for (const auto &[exit_code, stdout_output, stderr_output]: new_results) {
                TraceLog(
                        LOG_INFO,
                        "Pipeline output:\n\tStatus: %d\n\tStdout: \n\n%s\n\tStderr: \n%s\n",
                        exit_code, stdout_output.c_str(), stderr_output.c_str());
            }
        }


        if (IsKeyPressed(KEY_BACKSPACE)) {
            std::cout << "TODO: Remove last item from pipeline" << std::endl;
        }


        BeginDrawing();

        ClearBackground(Colors::BG1);

        Vector2 res{
                static_cast<float>(GetRenderWidth()),
                static_cast<float>(GetRenderHeight())
        };

        ImGui::pushFrame(res);

        // Accommodate for the draggable buttons
        float side_panel_width = 3 * 90 + 6 * padding;

        ImGui::beginPanel(side_panel_width);

        ImGui::pushButton("cat");
        ImGui::pushButton("grep");
        ImGui::pushButton("ls");

        ImGui::endPanel();

        ImGui::beginPanel(res.x - side_panel_width);

        ImGui::beginCMDBar(100, item, type_info);

        if (ImGui::pushRoundIconButton("run", 40) || IsKeyPressed(KEY_ENTER)) {
            pipeline.addPipelineItem(item);
            processor.pushTask(pipeline);

            item = PipelineItem();
        }

        ImGui::beginOutputPanel();

        for (const auto &output_result: output_results) {
            ImGui::pushOutputResult(output_result);
        }


        ImGui::endPanel();

        ImGui::endFrame();

        EndDrawing();
    }

    processor.stopThread();
    ImGui::clear();
    CloseWindow();
}