#pragma once

#include <raylib.h>
#include <string>
#include "commandHandler.hpp"

const int padding = 20;

namespace ImGui {
    struct ImGuiState {
        Rectangle current_frame{};
        Vector2 at{0, 0};
        int current_id = -1;
        int dragged = -1;
        Rectangle dragged_frame{};
        std::string dragged_cmd_name{};
        Vector2 anchor{};
        Vector2 res{};
    };

    void load_texture(const std::basic_string<char>& name, const char *fileName);

    ImGuiState &get_state();

    bool push_round_icon_button(std::string const & icon_name, float radius);

    void push_frame(Vector2 res);

    void begin_panel(float width);
    void end_panel();

    void push_button(std::string const &cmd);

    void end_frame();

    void begin_cmd_bar(float margin_right, PipelineItem &pipeline_item,  std::unordered_map<std::string, std::pair<CommandType, CommandArgType>> &type_info);

    // destroy any raylib shit
    void clear();
}
