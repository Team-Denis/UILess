#include <raylib.h>
#include <raymath.h>
#include <vector>
#include <chrono>
#include <iostream>

#include "commandHandler.hpp"
#include "cmdThread.hpp"
#include "result.hpp"
#include "colors.hpp"

#define WIDTH 1280
#define HEIGHT 800

const int padding = 20;

// TODO: Put in cpp/h files
// TODO: Rely less on hardcoded button sizes or at least make it a constant
namespace ImGui {
    struct ImGuiState {
        Rectangle current_frame{};
        Vector2 at{0, 0};
        int current_id = -1;
        int dragged = -1;
        Texture dragged_emoji{};
        Rectangle dragged_frame{};
        Vector2 anchor{};
    };

    static ImGuiState state;

    ImGuiState &get_state() {
        return state;
    }

    void draw_button(Texture emoji, Rectangle frame) {
        Rectangle emoji_frame{20 + frame.x, 10 + frame.y, 50, 50};

        DrawRectangleRounded(frame, 0.1f, 20, Colors::BLUE2);

        DrawTexturePro(emoji,
                       Rectangle{
                           0, 0, static_cast<float>(emoji.width),
                           static_cast<float>(emoji.height)
                       },
                       emoji_frame,
                       Vector2{0, 0},
                       0,
                       WHITE);
    }

    bool push_round_icon_button(Texture icon, float radius) {
        Vector2 center{state.at.x + radius, state.at.y + radius};

        auto mouse = GetMousePosition();
        bool does_collide = CheckCollisionPointCircle(mouse, center, radius);

        Rectangle icon_frame{center.x - 25 + 2, center.y - 25, 50, 50};

        DrawCircleV(center, radius, Colors::BG4);

        DrawTexturePro(icon,
                       Rectangle{
                           0, 0, static_cast<float>(icon.width),
                           static_cast<float>(icon.height)
                       },
                       icon_frame,
                       Vector2{0, 0},
                       0,
                       Colors::GREEN1);

        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && does_collide;
    }

    void push_frame() {
        state.current_frame = {};
        state.at = Vector2{0, 0};
        state.current_id = 0;
    }

    void begin_panel(float width) {
        state.current_frame = Rectangle{
            state.at.x + padding,
            state.at.y + padding,
            width - 2 * padding,
            HEIGHT - 2 * padding
        };

        state.at = Vector2{state.current_frame.x + padding, state.current_frame.y + padding};

        DrawRectangleRounded(state.current_frame, 0.04f, 20, Colors::BG2);
    }

    void end_panel() {
        state.at.x = state.current_frame.x + state.current_frame.width + padding;
        state.at.y = 0;
    }

    void push_button(Texture emoji) {
        float next_width = state.at.x + padding + 90;

        if (next_width > state.current_frame.width + state.current_frame.x) {
            state.at.x = state.current_frame.x + padding;
            state.at.y += 70 + padding;
        }

        auto mouse = GetMousePosition();

        Rectangle frame{state.at.x, state.at.y, 90, 70};
        bool hover = CheckCollisionPointRec(mouse, frame);

        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            TraceLog(LOG_INFO, "Dragged");

            if (state.dragged == -1) {
                state.dragged = state.current_id;
                state.dragged_emoji = emoji;
                state.anchor = Vector2Subtract(state.at, mouse);
            }
        }

        if (state.dragged != state.current_id) {
            draw_button(emoji, frame);
        } else {
            state.dragged_frame = Rectangle{
                mouse.x + state.anchor.x,
                mouse.y + state.anchor.y,
                frame.width,
                frame.height,
            };
        }

        state.at.x += 90 + padding;
        state.current_id++;
    }

    void end_frame() {
        if (state.dragged != -1) {
            draw_button(state.dragged_emoji, state.dragged_frame);
        }
    }

    void begin_cmd_bar(float margin_right, CommandPipeline &pipeline, ThreadSafeCmdProcessor &processor) {
        ImGuiState &state = get_state();
        state.at.y = HEIGHT - 100 - 2 * padding;
        float width = state.current_frame.width - 3 * padding - margin_right;
        Rectangle frame{state.at.x, state.at.y, width, 100};

        bool collision = false;

        if (state.current_id != -1) {
            collision = CheckCollisionRecs(frame, state.dragged_frame);
        }

        // Draw the command bar background
        DrawRectangleRounded(frame, 0.15f, 20, (collision && state.dragged != -1) ? Colors::BG4 : Colors::BG3);

        // Handle command drop
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state.dragged != -1) {
            state.dragged = -1;
            if (collision) {
                TraceLog(LOG_INFO, "Dropped");

                // Create a StartCommand with "ls"
                StartCommand cmd("ls", StringList{}, 0);
                PipelineItem pipeline_item;
                pipeline_item.setStartCommand(cmd);

                // Add the PipelineItem to the CommandPipeline
                pipeline.addPipelineItem(pipeline_item);
            }
        }

        // Visual representation of the pipeline commands
        int pipeline_index = 1;
        float block_size = 30.0f; // Width and height of each command block
        float block_spacing = 10.0f; // Space between blocks
        float start_x = frame.x + 10.0f; // Starting x position
        float start_y = frame.y + (frame.height - block_size) / 2.0f; // Centered y position

        for (const auto &item : pipeline) {
            std::optional<StartCommand> start_cmd = item.getStartCommand();
            std::string cmd_str = start_cmd ? start_cmd->getCmd() : "N/A";

            // Define the rectangle for the command block
            Rectangle cmd_block = {
                start_x + (pipeline_index - 1) * (block_size + block_spacing),
                start_y,
                block_size,
                block_size
            };

            // Choose color based on the command type or any other criteria
            Color cmd_color = Colors::BLUE2; // You can vary colors based on cmd_str if needed

            // Draw the command block
            DrawRectangleRounded(cmd_block, 0.1f, 5, cmd_color);

            // Optionally, draw an icon or abbreviation inside the block
            // For simplicity, we'll draw the first letter of the command
            std::string cmd_label = cmd_str.substr(0, 1); // First character
            DrawText(cmd_label.c_str(),
                     cmd_block.x + cmd_block.width / 2.0f - 5,
                     cmd_block.y + cmd_block.height / 2.0f - 10,
                     20,
                     Colors::FG1);

            pipeline_index++;
        }

        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (CheckCollisionPointRec(GetMousePosition(), frame)) {
                std::string path(droppedFiles.paths[0]);
                TraceLog(LOG_INFO, "%s", path.c_str());
                // Extend this to handle file paths as needed
            }

            UnloadDroppedFiles(droppedFiles);
        }

        // Update the x position for the next UI element
        state.at.x += width + padding;

        // Add a Run button to execute the pipeline
        Rectangle run_button = {frame.x + width - 80, frame.y + 30, 70, 40};
        DrawRectangleRounded(run_button, 0.1f, 5, Colors::GREEN1);
        DrawText("Run", run_button.x + 15, run_button.y + 10, 20, Colors::FG1);

        if (CheckCollisionPointRec(GetMousePosition(), run_button) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            TraceLog(LOG_INFO, "Run button clicked");
            // Push the pipeline as a task to the processor
            processor.pushTask(pipeline);
        }
    }
}

Texture load_texture(char const *fileName) {
    Texture tex = LoadTexture(fileName);
    GenTextureMipmaps(&tex);
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    return tex;
}

int main() {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(WIDTH, HEIGHT, "UILess");
    SetWindowState(FLAG_VSYNC_HINT);

    // Load shaders or other resources if needed
    // Shader shader = LoadShader(nullptr, TextFormat("shaders/blur.glsl", 330));

    CommandPipeline pipeline;

    Texture file = load_texture("assets/joy.png");
    Texture run_icon = load_texture("assets/run.png");

    // Initialize the ThreadSafeCmdProcessor
    ThreadSafeCmdProcessor processor;
    processor.startThread(); // Start the worker thread
    // Variables to handle results
    std::vector<Result> pending_results;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(Colors::BG1);

        ImGui::push_frame();

        float side_panel_width = 6 * padding + 90 * 6;

        ImGui::begin_panel(side_panel_width);

        // Add command buttons (all "ls" for now)
        for (int i = 0; i < 6; ++i) {
            ImGui::push_button(file);
        }

        ImGui::end_panel();

        ImGui::begin_panel(WIDTH - side_panel_width);

        ImGui::begin_cmd_bar(100, pipeline, processor);

        if (ImGui::push_round_icon_button(run_icon, 50)) {
            // Optionally handle button press if needed
        }

        ImGui::end_frame();

        EndDrawing();

        // Collect results from the processor
        if (processor.isResultAvailable()) {
            std::vector<Result> new_results = processor.popResults();
            pending_results.insert(pending_results.end(), new_results.begin(), new_results.end());

            // Print the results to cout
            for (const auto &res: new_results) {
                std::cout << "Pipeline Output:\n";
                std::cout << "  Exit Code: " << res.exit_code << "\n";
                std::cout << "  Stdout: " << res.stdout_output;
                std::cout << "  Stderr: " << res.stderr_output << "\n\n";
            }
        }
    }

    // Cleanup
    processor.stopThread();
    UnloadTexture(file);
    UnloadTexture(run_icon);
    CloseWindow();

    return 0;
}