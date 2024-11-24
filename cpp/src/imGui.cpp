#include <imGui.hpp>
#include <unordered_map>
#include <colors.hpp>
#include <raylib.h>
#include <raymath.h>

static std::unordered_map<std::string, Texture> strToTextureMap;

namespace ImGui {
    void load_texture(const std::basic_string<char> &name, const char *fileName) {
        Texture tex = LoadTexture(fileName);
        GenTextureMipmaps(&tex);
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
        strToTextureMap.emplace(name, tex);
    }

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

    bool push_round_icon_button(std::string const &icon_name, float radius) {
        Vector2 center{state.at.x + radius, state.at.y + radius};

        auto mouse = GetMousePosition();
        bool does_collide = CheckCollisionPointCircle(mouse, center, radius);

        Rectangle icon_frame{center.x - 25 + 2, center.y - 25, 50, 50};

        DrawCircleV(center, radius, Colors::BG4);

        auto icon = strToTextureMap.at(icon_name);

        DrawTexturePro(icon,
                       Rectangle{0, 0, static_cast<float>(icon.width),
                                 static_cast<float>(icon.height)},
                       icon_frame,
                       Vector2{0, 0},
                       0,
                       Colors::GREEN1);

        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && does_collide;
    }

    void push_frame(Vector2 res) {
        state.res = res;
        state.current_frame = {};
        state.at = Vector2{0, 0};
        state.current_id = 0;
    }

    void begin_panel(float width) {
        state.current_frame = Rectangle{
                state.at.x + padding,
                state.at.y + padding,
                width - 2 * padding,
                state.res.y - 2 * padding
        };

        state.at = Vector2{state.current_frame.x + padding, state.current_frame.y + padding};

        DrawRectangleRounded(state.current_frame, 0.04f, 20, Colors::BG2);
    }

    void end_panel() {
        state.at.x = state.current_frame.x + state.current_frame.width + padding;
        state.at.y = 0;
    }

    void push_button(std::string const &cmd) {
        auto emoji = strToTextureMap.at(cmd);

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
                state.dragged_cmd_name = cmd;
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
//
//    // TODO: Come with a better name than "button"
//    void push_button_cmd(Texture emoji) {
//        float next_width = state.at.x + padding + 90;
//
//        if (next_width > state.current_frame.width + state.current_frame.x) {
//            state.at.x = state.current_frame.x + padding;
//            state.at.y += 70 + padding;
//        }
//
//        auto mouse = GetMousePosition();
//
//        Rectangle frame{state.at.x, state.at.y, 90, 70};
//        bool hover = CheckCollisionPointRec(mouse, frame);
//
//        if (hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
//            TraceLog(LOG_INFO, "Dragged");
//
//            if (state.dragged == -1) {
//                state.dragged = state.current_id;
//                state.dragged_cmd_name = cmd;
//                state.anchor = Vector2Subtract(state.at, mouse);
//            }
//        }
//
//        if (state.dragged != state.current_id) {
//            draw_button(emoji, frame);
//        } else {
//            state.dragged_frame = Rectangle{
//                    mouse.x + state.anchor.x,
//                    mouse.y + state.anchor.y,
//                    frame.width,
//                    frame.height,
//            };
//        }
//
//        state.at.x += 90 + padding;
//        state.current_id++;
//    }

    void end_frame() {
        // Draw things that are supposed to be on top

        if (state.dragged != -1) {
            draw_button(strToTextureMap.at(state.dragged_cmd_name), state.dragged_frame);
        }
    }

    void begin_cmd_bar(float margin_right, PipelineItem &pipeline_item,
                       std::unordered_map<std::string, std::pair<CommandType, CommandArgType>> &type_info) {
        float width = state.current_frame.width - 3 * padding - margin_right;
        Rectangle frame{state.at.x, state.at.y, width, (2 * padding + 70)};

        bool collision = false;

        if (state.dragged != -1) {
            collision = CheckCollisionRecs(frame, state.dragged_frame);
        }

        DrawRectangleRounded(frame, 0.15f, 20, (collision && state.dragged != -1) ? Colors::BG4 : Colors::BG3);

        // Handle normal drop
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state.dragged != -1) {
            state.dragged = -1;
            if (collision) {
                TraceLog(LOG_INFO, "Dropped");

                std::string cmd_name = state.dragged_cmd_name;

                auto info = type_info.at(std::string(state.dragged_cmd_name));

                CommandArg arg{};
                arg.type = info.second;

                int res = pipeline_item.pushCommand(Command(cmd_name, info.first, arg));

                if (res == -1) {
                    TraceLog(LOG_INFO, "Incompatible item");
                }
            }
        }

        // Handle file drop
        if (IsFileDropped()) {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (CheckCollisionPointRec(GetMousePosition(), frame)) {
                // TODO: Read/Write File Cmd
                std::string path(droppedFiles.paths[0]);
                TraceLog(LOG_INFO, "%s", path.c_str());
            }

            UnloadDroppedFiles(droppedFiles);
        }

        float old_x = state.at.x;

        state.at = Vector2AddValue(state.at, padding);

        std::optional<Command> start_cmd = pipeline_item.getStartCommand();
        std::optional<Command> end_cmd = pipeline_item.getEndCommand();

        if (start_cmd.has_value()) {
            push_button(start_cmd->getName());
        }

        for (Command val: pipeline_item.getMiddleCommands()) {
            push_button(val.getName());
        }

        if (end_cmd.has_value()) {
            push_button(start_cmd->getName());
        }

        state.at.x = old_x + width + padding;
        state.at.y -= padding;

        // TODO: put start button logic here
    };

    void clear() {
        for (auto &[key, tex]: strToTextureMap) {
            UnloadTexture(tex);
        }

        strToTextureMap.clear();
    }
}

