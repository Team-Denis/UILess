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

        // TODO: Refactor dragged state
        int dragged = -1;
        Rectangle dragged_frame{};
        CommandType dragged_type;
        CommandArgType dragged_arg_type;
        std::string dragged_cmd_name{};
        Vector2 anchor{};
        Vector2 res{};
    };

    void loadTexture(const std::basic_string<char>& name, const char *fileName);

    ImGuiState &getState();

    bool pushRoundIconButton(std::string const & icon_name, float radius);

    void pushFrame(Vector2 res);

    void beginPanel(float width);
    void endPanel();

    void pushButton(std::string const &cmd, CommandType type, CommandArgType arg_type);

    void endFrame();

    void beginCMDBar(float margin_right, PipelineItem &pipeline_item);

    // destroy any raylib shit
    void clear();
}
