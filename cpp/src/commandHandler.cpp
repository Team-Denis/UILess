#include <algorithm>
#include <utility>
#include <raylib.h>

#include "commandHandler.hpp"

#include <iostream>

// Initialize Commands (ensure this is called before using CommandHandler)
extern void initializeCommands();


nlohmann::json PipelineItem::asJSON() const {
    nlohmann::json json_item;

    if (m_start_command.has_value()) {
        std::cout << "BBBB" << std::endl;
        json_item.merge_patch(m_start_command->asJSON());
    }

    if (!m_middle_commands.empty()) {
        std::cout << "AAA" << std::endl;

        nlohmann::json mdcmds_json = nlohmann::json::array();
        for (const auto& cmd : m_middle_commands) {
            mdcmds_json.emplace_back(cmd.asJSON());
        }
        json_item["mdcmd"] = mdcmds_json;
    }

    if (m_end_command.has_value()) {
        std::cout << "CCCC" << std::endl;
        json_item.merge_patch(m_end_command->asJSON());
    }

    std::cout << "AAAAAHELLO: " << to_string(json_item) << std::endl;

    return json_item;
}

int PipelineItem::pushCommand(const Command &cmd) {
    switch (m_state) {
        case PipeLineItemState::Start:
            TraceLog(LOG_INFO, "START");

            if (cmd.getType() != CommandType::Start) {
                return -1;
            }
            m_state = PipeLineItemState::Middle;
            m_start_command = cmd;
            return 0;
        case PipeLineItemState::Middle:
            TraceLog(LOG_INFO, "MIDDLE");
            if (cmd.getType() != CommandType::Middle) {
                return -1;
            }
            m_middle_commands.push_back(cmd);
            return 0;
        case PipeLineItemState::End:
            TraceLog(LOG_INFO, "END");
            if (cmd.getType() != CommandType::Middle) {
                return -1;
            }

            m_state = PipeLineItemState::End;
            m_end_command = cmd;
            return 0;
    }
}

// CommandPipeline Implementation
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

    std::cout << to_string(pipeline_json) << std::endl;

    return { {"pipeline", pipeline_json} };
}

// CommandHandler Implementation
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
    switch (m_arg.type) {
        case CommandArgType::None:
            return nlohmann::json::array({});
        case CommandArgType::Filepath:
        case CommandArgType::Text:
            assert(m_arg.value.has_value());
            return nlohmann::json::array({m_arg.value.value()});
    }
}

nlohmann::json Command::asJSON() const {
    nlohmann::json json;
    json["cmd"] = m_name;
    json["args"] = argsAsJSON();
    json["stream"] = 0.0;

    switch (m_type) {
        case CommandType::Start:
            return { "stcmd", json };
        case CommandType::Middle:
            return { "mdcmd", json };
        case CommandType::End:
            return { "edcmd", json };
    }

}
