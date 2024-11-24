#include <colors.hpp>
#include <raylib.h>
#include <commandHandler.hpp>
#include <unordered_map>
#include <vector>

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

    CommandPipeline pipeline;

    // Utility textures
    ImGui::loadTexture("run",  "assets/run.png");

    // Command textures
    ImGui::loadTexture("ls",         "assets/mag.png");
    ImGui::loadTexture("cat",        "assets/cat.png");
    ImGui::loadTexture("grep",       "assets/grapes.png");
    ImGui::loadTexture("rm",         "assets/wastebasket.png");
    ImGui::loadTexture("mkdir",      "assets/open_file_folder.png");
    ImGui::loadTexture("cd",         "assets/cd.png");
    ImGui::loadTexture("ifconfig",   "assets/globe_with_meridians.png");
    ImGui::loadTexture("ping",       "assets/ping_pong.png");
    ImGui::loadTexture("touch",      "assets/point_right.png");
    ImGui::loadTexture("neofetch",   "assets/muscle.png");
    ImGui::loadTexture("wc",         "assets/scroll.png");
    ImGui::loadTexture("ps",         "assets/ledger.png");
    ImGui::loadTexture("FILEWRITE",  "assets/pencil.png");

    ThreadSafeCmdProcessor processor;
    processor.startThread(); // Start the worker thread
    // Variables to handle results
    std::vector<Result> pending_results;

    PipelineItem item;

    // int resolutionLoc = GetShaderLocation(shader, "resolution");

    while (!WindowShouldClose()) {
        if (processor.isResultAvailable()) {
            std::vector<Result> new_results = processor.popResults();
            pending_results.insert(pending_results.end(), new_results.begin(), new_results.end());

            for (const auto &[exit_code, stdout_output, stderr_output]: new_results) {
                TraceLog(
                        LOG_INFO,
                        "Pipeline output:\n\tStatus: %d\n\tStdout: \n\n%s\n\tStderr: \n%s\n",
                        exit_code, stdout_output.c_str(), stderr_output.c_str());
            }
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

        ImGui::pushButton("ls",         CommandType::Start,     CommandArgType::None);
        ImGui::pushButton("cat",        CommandType::Start,     CommandArgType::Filepath);
        ImGui::pushButton("grep",       CommandType::Middle,    CommandArgType::Text);
        ImGui::pushButton("rm",         CommandType::Start,     CommandArgType::Filepath);
        ImGui::pushButton("mkdir",      CommandType::Start,     CommandArgType::Filepath);
        ImGui::pushButton("cd",         CommandType::Start,     CommandArgType::Filepath);
        ImGui::pushButton("ifconfig",   CommandType::Start,     CommandArgType::None);
        ImGui::pushButton("ping",       CommandType::Start,     CommandArgType::Text);
        ImGui::pushButton("touch",      CommandType::Start,     CommandArgType::Filepath);
        ImGui::pushButton("wc",         CommandType::Middle,    CommandArgType::Filepath);
        ImGui::pushButton("neofetch",   CommandType::Start,     CommandArgType::None);
        ImGui::pushButton("ps",         CommandType::Start,     CommandArgType::None);
        ImGui::pushButton("FILEWRITE",  CommandType::End,       CommandArgType::Filepath);

        ImGui::endPanel();

        ImGui::beginPanel(res.x - side_panel_width);

        ImGui::beginCMDBar(100, item);

        if (ImGui::pushRoundIconButton("run", 40)) {
            pipeline.addPipelineItem(item);
            processor.pushTask(pipeline);
        }

        ImGui::endPanel();

        ImGui::endFrame();

        EndDrawing();
    }

    processor.stopThread();
    ImGui::clear();
    CloseWindow();
}