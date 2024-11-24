#include <iostream>

#include <raylib.h>

#include "commandHandler.hpp"

extern void initializeCommands();

nlohmann::json PipelineItem::asJSON() const {
    nlohmann::json json_item;

    if (m_start_command.has_value()) {
        json_item["stcmd"] = m_start_command->asJSON();
    }

    if (!m_middle_commands.empty()) {
        nlohmann::json mdcmds_json = nlohmann::json::array();
        for (const auto &cmd: m_middle_commands) {
            mdcmds_json.emplace_back(cmd.asJSON());
        }
        json_item["mdcmd"] = mdcmds_json;
    }

    if (m_end_command.has_value()) {
        json_item["edcmd"] = m_end_command->asJSON();
    }

    std::cout << "PipelineItem JSON: " << json_item.dump() << std::endl;

    return json_item;
}

int PipelineItem::pushCommand(const Command &cmd) {
    switch (m_state) {
        case PipeLineItemState::Start:
            TraceLog(LOG_INFO, "START");

            if (cmd.getType() != CommandType::Start) {
                TraceLog(LOG_WARNING, "Expected Start Command.");
                return -1;
            }

            m_start_command = cmd;
            m_state = PipeLineItemState::Middle;
            return 0;

        case PipeLineItemState::Middle: TraceLog(LOG_INFO, "MIDDLE");

            if (cmd.getType() == CommandType::Middle) {
                m_middle_commands.push_back(cmd);
                return 0;
            }
            if (cmd.getType() == CommandType::End) {
                m_end_command = cmd;
                m_state = PipeLineItemState::End;
                return 0;
            }
            TraceLog(LOG_WARNING, "Invalid Command Type in Middle State.");

            return -1;

        case PipeLineItemState::End: TraceLog(LOG_INFO, "END");

            TraceLog(LOG_WARNING, "Cannot add commands after End Command.");
            return -1;

        default:
            TraceLog(LOG_ERROR, "Unknown PipelineItemState.");
            return -1;
    }
}

void CommandPipeline::addPipelineItem(const PipelineItem& item) {
    pipeline_items.push_back(item);
    std::cout << "CommandPipeline: Added PipelineItem with "
              << (item.getStartCommand().has_value() ? "StartCommand, " : "")
              << (!item.getMiddleCommands().empty() ? std::to_string(item.getMiddleCommands().size()) + " MiddleCommands, " : "")
              << (item.getEndCommand().has_value() ? "EndCommand" : "")
              << std::endl; // Debug
}

nlohmann::json CommandPipeline::asJSON() const {
    nlohmann::json pipeline_json = nlohmann::json::array();

    for (const auto& item : pipeline_items) {
        pipeline_json.emplace_back(item.asJSON());
    }

    std::cout << "CommandPipeline JSON: " << pipeline_json.dump() << std::endl;

    return { {"pipeline", pipeline_json} };
}

CommandHandler::CommandHandler() {
    static bool initialized = false;

    if (!initialized) {
        initializeCommands();
        initialized = true;
    }
}

const CommandPipeline& CommandHandler::getPipeline() const {
    return pipeline;
}

nlohmann::json Command::argsAsJSON() const {
    nlohmann::json args_json = nlohmann::json::array();

    if (m_arg.type == CommandArgType::Filepath || m_arg.type == CommandArgType::Text) {
        if (m_arg.value.has_value()) {
            args_json.emplace_back(m_arg.value.value());
        }
    }

    return args_json;
}

nlohmann::json Command::asJSON() const {
    nlohmann::json json;
    json["cmd"] = m_name;
    json["args"] = argsAsJSON();
    json["stream"] = 0.0;

    switch (m_type) {
        case CommandType::Start:
            json["type"] = "stcmd";
        break;
        case CommandType::Middle:
            json["type"] = "mdcmd";
        break;
        case CommandType::End:
            json["type"] = "edcmd";
        break;
    }

    return json;
}