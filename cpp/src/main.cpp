#include <colors.hpp>
#include <raylib.h>
#include <commandHandler.hpp>
#include <unordered_map>
#include <vector>
#include <raymath.h>

#include "cmdThread.hpp"
#include "imGui.hpp"

const int max_output_result = 5;

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
    ImGui::loadTexture("curl",       "assets/muscle.png");
    ImGui::loadTexture("wc",         "assets/scroll.png");
    ImGui::loadTexture("ps",         "assets/ledger.png");
    ImGui::loadTexture("FILEWRITE",  "assets/pencil.png");

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
            if (item.end_command.has_value()) {
                printf("Deleting end command\n");
                item.deleteEndCommand();
            }
            else if (!item.middle_commands.empty()) {
                printf("Deleting middle command\n");
                item.deleteMiddleCommand(item.middle_commands.size() - 1);
            }
            else if (item.start_command.has_value()) {
                printf("Deleting start command\n");
                item.deleteStartCommand();
                item = PipelineItem();
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
        ImGui::pushButton("ifconfig",   CommandType::Start,     CommandArgType::None);
        ImGui::pushButton("ping",       CommandType::Start,     CommandArgType::Text);
        ImGui::pushButton("touch",      CommandType::Start,     CommandArgType::Filepath);
        ImGui::pushButton("wc",         CommandType::Middle,    CommandArgType::None);
        ImGui::pushButton("curl",       CommandType::Start,     CommandArgType::Text);
        ImGui::pushButton("ps",         CommandType::Start,     CommandArgType::None);
        ImGui::pushButton("FILEWRITE",  CommandType::End,       CommandArgType::NewFilepath);

        ImGui::endPanel();

        ImGui::beginPanel(res.x - side_panel_width);

        ImGui::beginCMDBar(item);

        ImGui::beginOutputPanel();

        for (const auto &output_result: output_results) {
            ImGui::pushOutputResult(output_result);
        }

        if (ImGui::pushActionButton("run", 40, Vector2SubtractValue(res, 80)) || IsKeyPressed(KEY_ENTER)) {
            bool is_valid = item.start_command.has_value();

            if (item.start_command.has_value()) {
                if (!item.start_command->isComplete()) {
                    is_valid = false;
                }
            }

            if (item.end_command.has_value()) {
                if (!item.end_command->isComplete()) {
                    is_valid = false;
                }
            }


            for (auto &cmd : item.middle_commands) {
                if (!cmd.isComplete()) {
                    is_valid = false;
                }
            }

            if (is_valid) {
                pipeline.addPipelineItem(item);
                processor.pushTask(pipeline);

                item = PipelineItem();
            }
        }


        ImGui::endPanel();

        ImGui::endFrame();

        EndDrawing();
    }

    processor.stopThread();
    ImGui::clear();
    CloseWindow();
}