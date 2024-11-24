#include <unordered_map>

#include <raylib.h>
#include <raymath.h>

#include <imGui.hpp>
#include <colors.hpp>
#include "osHelper.hpp"

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

    void drawButton(Texture emoji, Rectangle frame, bool error = false) {
        Rectangle emoji_frame{20 + frame.x, 10 + frame.y, 50, 50};

        float radius = 0.2f;

        if (error) {
            // FIXME: Change to official color
            DrawRectangleRounded(frame, radius, 20, RED);
            frame.x += 4;
            frame.y += 4;
            frame.width -= 8;
            frame.height -= 8;
            radius = 0.15f;
        }

        DrawRectangleRounded(frame, radius, 20, Colors::BLUE2);


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
        auto emoji = strToTextureMap.at(cmd.name);

        if (float next_width = state.at.x + padding + 90; next_width > state.current_frame.width + state.current_frame.x) {
            state.at.x = state.current_frame.x + padding;
            state.at.y += 70 + padding;
        }

        auto mouse = GetMousePosition();

        Rectangle frame{state.at.x, state.at.y, 90, 70};

        if (bool hover = CheckCollisionPointRec(mouse, frame); hover && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            std::string res;
            switch (cmd.arg.type) {
                case CommandArgType::None:
                    break;
                case CommandArgType::Text:
//                    openTextDialog(res);
//                    cmd.arg.value = std::move(res);
                    break;
                case CommandArgType::Filepath:
                    openFileDialog(res);
                    res.pop_back();
                    TraceLog(LOG_INFO, "Path %s", res.c_str());
                    cmd.arg.value = std::move(res);

                    break;
            }
        }

        drawButton(emoji, frame, !cmd.isComplete());

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

        if (pipeline_item.start_command.has_value()) {
            pushCMDButton(pipeline_item.start_command.value());
        }

        int potential_middle_insert = -1;

        for (size_t i = 0; i < pipeline_item.middle_commands.size(); i++) {
            Command &val = pipeline_item.middle_commands[i];

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

        if (pipeline_item.end_command.has_value()) {
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
                    potential_middle_insert = pipeline_item.middle_commands.size();
                }
            }

            pushCMDButton(pipeline_item.end_command.value());
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


//        state.at.x = old_x + width + padding;
//        state.at.y -= padding;
//
        // Place "at" at the bottom of the cmd bar
        state.at.x = state.current_frame.x + padding;
        state.at.y = state.current_frame.y + 110 + padding;
    }

    void clear() {
        for (auto &tex: strToTextureMap | std::views::values) {
            UnloadTexture(tex);
        }

        strToTextureMap.clear();
    }

    void beginOutputPanel() {
        Rectangle frame = {state.at.x, state.at.y + padding, state.current_frame.width - 2*padding, state.current_frame.height - state.at.y - padding};
//        DrawRectangleRounded(frame, 0.04f, 20, Colors::BG3);
        state.at.y += padding;
    }

    std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> tokens;
        std::istringstream stream(str);
        std::string token;

        while (std::getline(stream, token, delimiter)) {
            tokens.push_back(token);
        }

        return tokens;
    }

    bool pushOutputResult(OutputResult output) {
        float width = state.current_frame.width - 4*padding;
        float height = 100;

        Rectangle rect = {state.at.x + padding, state.at.y + padding, width, height};

        // Check if the exit code is 0 (Green) or not (Blue)
        Color color = output.result.exit_code == 0 ? Colors::GREEN1 : Colors::BLUE1;

        DrawRectangleRounded(rect, 0.1f, 20, color);

        // Draw output text
        std::string text = output.result.stdout_output;
        int fontSize = 20;
        int lineHeight = fontSize + 5;
        int maxLinesPerColumn = 3;

        // Split text into multiple lines
        std::vector<std::string> lines = split(text, '\n');

        // Draw each line of text in columns
        int max_col_width = 0;
        int width_offset = 0;
        for (size_t i = 0; i < lines.size(); ++i) {

            if (i % maxLinesPerColumn == 0 && i != 0) {
                width_offset += max_col_width + padding;
                max_col_width = 0;
            }

            DrawText(lines[i].c_str(), rect.x + padding + width_offset, rect.y + padding + lineHeight * (i % maxLinesPerColumn), fontSize, WHITE);

            int m = MeasureText(lines[i].c_str(), fontSize);
            if (m > max_col_width) {
                max_col_width = m;
            }
        }


        // Draw time
        std::string time = std::format("{:02}:{:02}:{:02}", output.datetime.tm_hour, output.datetime.tm_min, output.datetime.tm_sec);
        DrawText(time.c_str(), rect.x + rect.width - MeasureText(time.c_str(), 20) - padding, rect.y + padding + 30, 20, WHITE);

        state.at.y += height + padding;

        return CheckCollisionPointRec(GetMousePosition(), rect);
    }

}