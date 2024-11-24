#include <unordered_map>

#include <raylib.h>
#include <raymath.h>

#include <imGui.hpp>
#include <colors.hpp>

static std::unordered_map<std::string, Texture> strToTextureMap;

namespace ImGui {
    void loadTexture(const std::basic_string<char> &name, const char *fileName) {
        Texture tex = LoadTexture(fileName);
        GenTextureMipmaps(&tex);
        SetTextureFilter(tex, TEXTURE_FILTER_BILINEAR);
        strToTextureMap.emplace(name, tex);
    }

    static ImGuiState state;

    ImGuiState &getState() {
        return state;
    }

    void drawButton(Texture emoji, Rectangle frame) {
        Rectangle emoji_frame{20 + frame.x, 10 + frame.y, 50, 50};

        DrawRectangleRounded(frame, 0.1f, 20, Colors::BLUE2);

        DrawTexturePro(emoji,
                       Rectangle{0, 0, static_cast<float>(emoji.width), static_cast<float>(emoji.height)},
                       emoji_frame,
                       Vector2{0, 0},
                       0,
                       WHITE);
    }

    bool pushActionButton(std::string const &icon_name, float radius, Vector2 center) {

        auto mouse = GetMousePosition();
        bool does_collide = CheckCollisionPointCircle(mouse, center, radius);

        Rectangle icon_frame{center.x - 25 + 2, center.y - 25, 50, 50};

        DrawCircleV(center, radius, Colors::BG4);

        auto icon = strToTextureMap.at(icon_name);

        DrawTexturePro(icon,
                       Rectangle{0, 0, static_cast<float>(icon.width), static_cast<float>(icon.height)},
                       icon_frame,
                       Vector2{0, 0},
                       0,
                       Colors::GREEN1);

        return IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && does_collide;
    }

    void pushFrame(Vector2 res) {
        state.res = res;
        state.current_frame = {};
        state.at = Vector2{0, 0};
        state.current_id = 0;
    }

    void beginPanel(float width) {
        state.current_frame = Rectangle{
                state.at.x + padding,
                state.at.y + padding,
                width - 2 * padding,
                state.res.y - 2 * padding
        };

        state.at = Vector2{state.current_frame.x + padding, state.current_frame.y + padding};

        DrawRectangleRounded(state.current_frame, 0.04f, 20, Colors::BG2);
    }

    void endPanel() {
        state.at.x = state.current_frame.x + state.current_frame.width + padding;
        state.at.y = 0;
    }

    void pushButton(std::string const &cmd, CommandType type, CommandArgType arg_type) {
        auto emoji = strToTextureMap.at(cmd);

        if (float next_width = state.at.x + padding + 90; next_width > state.current_frame.width + state.current_frame.x) {
            state.at.x = state.current_frame.x + padding;
            state.at.y += 70 + padding;
        }

        auto mouse = GetMousePosition();

        Rectangle frame{state.at.x, state.at.y, 90, 70};

        if (bool hover = CheckCollisionPointRec(mouse, frame); hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            TraceLog(LOG_INFO, "Dragged");

            if (state.dragged == -1) {
                state.dragged = state.current_id;
                state.dragged_cmd_name = cmd;
                state.dragged_type = type;
                state.dragged_arg_type = arg_type;
                state.anchor = Vector2Subtract(state.at, mouse);
            }
        }

        if (state.dragged != state.current_id) {
            drawButton(emoji, frame);
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

    void pushCMDButton(Command &cmd) {
        auto emoji = strToTextureMap.at(cmd.getName());

        if (float next_width = state.at.x + padding + 90; next_width > state.current_frame.width + state.current_frame.x) {
            state.at.x = state.current_frame.x + padding;
            state.at.y += 70 + padding;
        }

        auto mouse = GetMousePosition();

        Rectangle frame{state.at.x, state.at.y, 90, 70};

        if (bool hover = CheckCollisionPointRec(mouse, frame); hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            // No drag
        }

        drawButton(emoji, frame);

        state.at.x += 90 + padding;
        state.current_id++;
    }

    void endFrame() {
        // Draw things that are supposed to be on top
        if (state.dragged != -1) {
            drawButton(strToTextureMap.at(state.dragged_cmd_name), state.dragged_frame);
        }
    }

    void beginCMDBar(PipelineItem &pipeline_item) {
        float width = state.current_frame.width - 2 * padding;
        Rectangle frame{state.at.x, state.at.y, width, (2 * padding + 70)};

        bool collision = false;

        if (state.dragged != -1) {
            collision = CheckCollisionRecs(frame, state.dragged_frame);
        }

        DrawRectangleRounded(frame, 0.15f, 20, collision && state.dragged != -1 ? Colors::BG4 : Colors::BG3);

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
            pushCMDButton(start_cmd.value());
        }

        int potential_middle_insert = -1;

        for (size_t i = 0; i < pipeline_item.getMiddleCommands().size(); i++) {
            Command val = pipeline_item.getMiddleCommands()[i];

            Rectangle check_rec {
                    state.at.x - padding - 4,
                    state.at.y,
                    90 + padding,
                    70,
            };

            if (state.dragged != -1) {
                Vector2 dragged_center {
                        state.dragged_frame.x + state.dragged_frame.width / 2.0f,
                        state.dragged_frame.y + state.dragged_frame.height / 2.0f,
                };

                if (CheckCollisionPointRec(dragged_center, check_rec)) {
                    state.at.x += 90 + padding;
                    potential_middle_insert = i;
                }
            }

            pushCMDButton(val);
        }

        if (end_cmd.has_value()) {
            // TODO: Remove duplicated code
            Rectangle check_rec {
                    state.at.x - padding - 4,
                    state.at.y,
                    90 + padding,
                    70,
            };

            if (state.dragged != -1) {
                Vector2 dragged_center {
                        state.dragged_frame.x + state.dragged_frame.width / 2.0f,
                        state.dragged_frame.y + state.dragged_frame.height / 2.0f,
                };

                if (CheckCollisionPointRec(dragged_center, check_rec)) {
                    state.at.x += 90 + padding;
                    potential_middle_insert = pipeline_item.getMiddleCommands().size();
                }
            }

            pushCMDButton(end_cmd.value());
        }

        // Handle normal drop
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && state.dragged != -1) {
            state.dragged = -1;

            if (collision) {
                TraceLog(LOG_INFO, "Dropped");

                std::string cmd_name = state.dragged_cmd_name;

                CommandArg arg{};
                arg.type = state.dragged_arg_type;

                auto cmd = Command(cmd_name, state.dragged_type, arg);

                if (potential_middle_insert != -1) {
                    pipeline_item.insertMiddleCommand(cmd, potential_middle_insert);
                } else {
                    if (int res = pipeline_item.pushCommand(cmd); res == -1) {
                        TraceLog(LOG_INFO, "Incompatible item");
                    }
                }
            }
        }


        state.at.x = old_x + width + padding;
        state.at.y -= padding;

        // TODO: put start button logic here
    }

    void clear() {
        for (auto &tex: strToTextureMap | std::views::values) {
            UnloadTexture(tex);
        }

        strToTextureMap.clear();
    }
}