#include <colors.h>
#include <raylib.h>
#include <raymath.h>
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

    bool draw_run_button(Vector2 center, float radius) {
        auto mouse = GetMousePosition();
        bool does_collide = CheckCollisionPointCircle(mouse, center, radius);

        DrawCircleV(center, radius, Colors::GREEN1);

        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && does_collide;
    }

    void push_frame() {
        state.current_frame = {};
        state.at = Vector2 {0, 0};
        state.current_id = 0;
    }

    void begin_panel(float width) {
        state.current_frame = Rectangle {
            state.at.x + padding,
            state.at.y + padding,
            width - 2 * padding,
            HEIGHT - 2 * padding
        };

        state.at = Vector2 {state.current_frame.x + padding, state.current_frame.y + padding};

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
            state.dragged_frame = Rectangle {
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

        auto mouse = GetMousePosition();

        if (state.dragged != -1) {
            draw_button(state.dragged_emoji, state.dragged_frame);
        }
    }

    void begin_cmd_bar(float margin_right) {
        state.at.y = HEIGHT - 100 - 2 * padding;
        float width = state.current_frame.width - 4 * padding - margin_right;
        Rectangle frame{state.at.x, state.at.y, width, 100};
        DrawRectangleRounded(frame, 0.15f, 20, Colors::BG3);

        bool collision = false;

        if (state.current_id != -1) {
            collision = CheckCollisionRecs(frame, state.dragged_frame);
        }

        // Handle drop
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state.dragged != -1) {
            state.dragged = -1;
            if (collision) {
                TraceLog(LOG_INFO, "Dropped");
            }
        }

        state.at.x = width + padding;
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


    auto file = load_texture("assets/file.png");

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

        ImGui::begin_cmd_bar(60);

        ImGui::end_panel();

        ImGui::end_frame();

        EndDrawing();
    }
}