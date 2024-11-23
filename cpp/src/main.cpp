#include <colors.hpp>
#include <raylib.h>
#include <raymath.h>
#include <pipelineRunner.hpp>
#include <commandHandler.hpp>
#include <unordered_map>
#include <vector>

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
                       Rectangle{0, 0, static_cast<float>(emoji.width),
                                 static_cast<float>(emoji.height)},
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
                       Rectangle{0, 0, static_cast<float>(icon.width),
                                 static_cast<float>(icon.height)},
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
        // Draw things that are supposed to be on top

        if (state.dragged != -1) {
            draw_button(state.dragged_emoji, state.dragged_frame);
        }
    }

    void begin_cmd_bar(float margin_right, CommandPipeline &pipeline) {
        state.at.y = HEIGHT - 100 - 2 * padding;
        float width = state.current_frame.width - 3 * padding - margin_right;
        Rectangle frame{state.at.x, state.at.y, width, 100};

        bool collision = false;

        if (state.current_id != -1) {
            collision = CheckCollisionRecs(frame, state.dragged_frame);
        }

        DrawRectangleRounded(frame, 0.15f, 20, (collision && state.dragged != -1) ? Colors::BG4 : Colors::BG3);

        std::shared_ptr<PipelineItem> pipeline_item = nullptr;

        // Handle normal drop
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state.dragged != -1) {
            state.dragged = -1;
            if (collision) {
                TraceLog(LOG_INFO, "Dropped");

                auto cmd = std::make_shared<StartCommand>("ls", StringList{}, 0);
                pipeline_item = std::make_shared<PipelineItem>();

                pipeline_item->set_start_command(cmd);
//                pipeline.set_parallel(false);

                pipeline.add_pipeline_item(pipeline_item);
            }
        }

        auto item = pipeline.first();

        if (item != nullptr) {
            auto start = item->get_start_command();

            if (start != nullptr) {
//                TraceLog(LOG_INFO, "Hello");
            }
        }

        // Handle file drop
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (CheckCollisionPointRec(GetMousePosition(), frame)) {
                std::string path(droppedFiles.paths[0]);
                TraceLog(LOG_INFO, "%s", path.c_str());
            }

            UnloadDroppedFiles(droppedFiles);
        }


        state.at.x += width + padding;
    }
};

Texture load_texture(char const *fileName) {
    Texture tex = LoadTexture(fileName);
    GenTextureMipmaps(&tex);
    SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);

    return tex;
}

int main(int argc, char **argv) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);

    InitWindow(WIDTH, HEIGHT, "UILess");

    SetWindowState(FLAG_VSYNC_HINT);

    Shader shader = LoadShader(nullptr, TextFormat("shaders/blur.glsl", 330));

    CommandPipeline pipeline;

    auto file = load_texture("assets/joy.png");
    auto run_icon = load_texture("assets/run.png");

    std::vector<int> cmds;

    int resolutionLoc = GetShaderLocation(shader, "resolution");

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(Colors::BG1);

        ImGui::push_frame();

        // Accommodate for the draggable buttons
        float side_panel_width = 3 * 90 + 6 * padding;

        ImGui::begin_panel(side_panel_width);

        ImGui::push_button(file);
        ImGui::push_button(file);
        ImGui::push_button(file);
        ImGui::push_button(file);
        ImGui::push_button(file);
        ImGui::push_button(file);

        ImGui::end_panel();

        ImGui::begin_panel(WIDTH - side_panel_width);

        ImGui::begin_cmd_bar(100, pipeline);

        ImGui::push_round_icon_button(run_icon, 50);

        ImGui::end_panel();

        ImGui::end_frame();

        EndDrawing();
    }
}