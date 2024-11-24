#pragma once

#include <raylib.h>
#include <string>
#include <stdlib.h>
#include <format>
#include <ctime>
#include "commandHandler.hpp"
#include "result.hpp"

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

    struct OutputResult {
        Result result;
        struct tm datetime;
    };

    void loadTexture(const std::basic_string<char>& name, const char *fileName);

    ImGuiState &getState();

    bool pushRoundIconButton(std::string const & icon_name, float radius);

    void pushFrame(Vector2 res);

    void beginPanel(float width);
    void endPanel();

    void pushButton(std::string const &cmd);

    void endFrame();

    void beginCMDBar(float margin_right, PipelineItem &pipeline_item,  std::unordered_map<std::string, std::pair<CommandType, CommandArgType>> &type_info);

    void beginOutputPanel();
    bool pushOutputResult(OutputResult output);

    // destroy any raylib shit
    void clear();
}
