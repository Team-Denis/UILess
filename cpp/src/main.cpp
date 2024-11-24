#include <colors.hpp>
#include <raylib.h>
#include <pipelineRunner.hpp>
#include <commandHandler.hpp>
#include <unordered_map>
#include <vector>

#include "cmdThread.hpp"
#include "imGui.hpp"

const int START_WIDTH = 1280;
const int START_HEIGHT = 800;

int main(int argc, char **argv) {
    // Prevents compiler from yapping
    (void)argc;
    (void)argv;

    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(START_WIDTH, START_HEIGHT, "UILess");
    SetWindowMinSize(START_WIDTH * 2 / 3, START_HEIGHT * 2 / 3);

    SetWindowState(FLAG_WINDOW_RESIZABLE | FLAG_VSYNC_HINT);

    SetTargetFPS(60);

//    Shader shader = LoadShader(nullptr, TextFormat("shaders/blur.glsl", 330));
    std::unordered_map<std::string, std::pair<CommandType, CommandArgType>> type_info;
    type_info.emplace("ls", std::pair{CommandType::Start, CommandArgType::None});
    type_info.emplace("cat", std::pair{CommandType::Start, CommandArgType::Filepath});
    type_info.emplace("grep", std::pair{CommandType::Middle, CommandArgType::Text});

    CommandPipeline pipeline;

    // Utility textures
    ImGui::load_texture("run", "assets/run.png");

    // Command textures
    ImGui::load_texture("ls", "assets/mag.png");
    ImGui::load_texture("cat", "assets/cat.png");
    ImGui::load_texture("grep", "assets/grapes.png");

    ThreadSafeCmdProcessor processor;
    processor.startThread(); // Start the worker thread
    // Variables to handle results
    std::vector<Result> pending_results;

    PipelineItem item;
    pipeline.addPipelineItem(item);

//    int resolutionLoc = GetShaderLocation(shader, "resolution");

    while (!WindowShouldClose()) {
        if (processor.isResultAvailable()) {
            std::vector<Result> new_results = processor.popResults();
            pending_results.insert(pending_results.end(), new_results.begin(), new_results.end());

            for (const auto &res: new_results) {
                TraceLog(
                        LOG_INFO,
                        "Pipeline output:\n\tStatus: %d\n\tStdout: %s\n\tStderr: %s\n",
                        res.exit_code, res.stdout_output.c_str(), res.stderr_output.c_str());
            }
        }

        BeginDrawing();

        ClearBackground(Colors::BG1);

        Vector2 res{
                static_cast<float>(GetRenderWidth()),
                static_cast<float>(GetRenderHeight())
        };

        ImGui::push_frame(res);

        // Accommodate for the draggable buttons
        float side_panel_width = 3 * 90 + 6 * padding;

        ImGui::begin_panel(side_panel_width);

        ImGui::push_button("cat");
        ImGui::push_button("grep");
        ImGui::push_button("ls");

        ImGui::end_panel();

        ImGui::begin_panel(res.x - side_panel_width);

        ImGui::begin_cmd_bar(100, item, type_info);

        if (ImGui::push_round_icon_button("run", 40)) {
            processor.pushTask(pipeline);
        }

        ImGui::end_panel();

        ImGui::end_frame();

        EndDrawing();
    }
    processor.stopThread();

    ImGui::clear();

    CloseWindow();
}