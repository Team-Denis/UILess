#include <iostream>

#include <raylib.h>

#include "commandHandler.hpp"

// External function to initialize all available commands (i have no clue how to do it otherwise)
extern void initializeCommands();

// Convert a PipelineItem instance to JSON format
nlohmann::json PipelineItem::asJSON() const {
    nlohmann::json json_item;

    if (start_command.has_value()) {
        json_item["stcmd"] = start_command->asJSON();
    }

    if (!middle_commands.empty()) {
        nlohmann::json mdcmds_json = nlohmann::json::array();
        for (const auto &cmd: middle_commands) {
            mdcmds_json.emplace_back(cmd.asJSON());
        }
        json_item["mdcmd"] = mdcmds_json;
    }

    if (end_command.has_value()) {
        json_item["edcmd"] = end_command->asJSON();
    }

    std::cout << "PipelineItem JSON: " << json_item.dump() << std::endl;

    return json_item;
}

// Push a command into the PipelineItem based on its state
int PipelineItem::pushCommand(const Command &cmd) {
    switch (m_state) {
        case PipeLineItemState::Start:
            TraceLog(LOG_INFO, "START");
            // Ensure the first command is a Start command
            if (cmd.cmd_type != CommandType::Start) {
                TraceLog(LOG_WARNING, "Expected Start Command.");
                return -1; // Early exit on unexpected command cmd_type
            }

            start_command = cmd;
            // Transitioning states manually – one wrong move and the whole pipeline collapses
            m_state = PipeLineItemState::Middle; // Transition to Middle state
            return 0;

        case PipeLineItemState::Middle: TraceLog(LOG_INFO, "MIDDLE");
            if (cmd.cmd_type == CommandType::Middle) {
                middle_commands.push_back(cmd);
                return 0;
            }

            if (cmd.cmd_type == CommandType::End) {
                end_command = cmd;
                m_state = PipeLineItemState::End; // Transition to End state
                return 0;
            }

            TraceLog(LOG_WARNING, "Invalid Command Type in Middle State.");
            return -1; // Invalid command cmd_type for Middle state

        case PipeLineItemState::End: TraceLog(LOG_INFO, "END");
            // No commands should be added after End
            TraceLog(LOG_WARNING, "Cannot add commands after End Command.");
            return -1;

        default:
            TraceLog(LOG_ERROR, "Unknown PipelineItemState.");
            return -1; // Catch-all for undefined states
    }
}

// Add a PipelineItem to the CommandPipeline
void CommandPipeline::addPipelineItem(const PipelineItem& item) {
    pipeline_items.push_back(item);
    // Overkill debugging to confirm the addition of the PipelineItem
    std::cout << "CommandPipeline: Added PipelineItem with "
              << (item.start_command.has_value() ? "StartCommand, " : "")
              << (!item.middle_commands.empty() ? std::to_string(item.middle_commands.size()) + " MiddleCommands, " : "")
              << (item.end_command.has_value() ? "EndCommand" : "")
              << std::endl; // Debug
}

// Convert the entire CommandPipeline to JSON
nlohmann::json CommandPipeline::asJSON() const {
    nlohmann::json pipeline_json = nlohmann::json::array();

    for (const auto& item : pipeline_items) {
        pipeline_json.emplace_back(item.asJSON());
    }
    // Debugging output to verify the entire pipeline's JSON
    std::cout << "CommandPipeline JSON: " << pipeline_json.dump() << std::endl;

    return { {"pipeline", pipeline_json} };
}

// Constructor for CommandHandler, ensures commands are initialized only once
CommandHandler::CommandHandler() {
    static bool initialized = false;

    if (!initialized) {
        initializeCommands();
        initialized = true; // Prevent re-initialization
    }
}

const CommandPipeline& CommandHandler::getPipeline() const {
    return pipeline;
}

// Convert command arguments to JSON format
nlohmann::json Command::argsAsJSON() const {
    nlohmann::json args_json = nlohmann::json::array();
    // Depending on the argument cmd_type, add it to the JSON array
    if (arg.type == CommandArgType::Filepath || arg.type == CommandArgType::Text) {
        if (arg.value.has_value()) {
            args_json.emplace_back(arg.value.value());
        }
    }

    return args_json;
}

nlohmann::json Command::asJSON() const {
    nlohmann::json json;
    json["cmd"] = name;
    json["args"] = argsAsJSON();
    json["stream"] = 0; // Placeholder for stream information

    switch (cmd_type) {
        case CommandType::Start:
            json["cmd_type"] = "stcmd";
        break;
        case CommandType::Middle:
            json["cmd_type"] = "mdcmd";
        break;
        case CommandType::End:
            json["cmd_type"] = "edcmd";
        break;
    }

    return json;
}
void PipelineItem::insertMiddleCommand(const Command &cmd, int relative_index) {
    if (cmd.cmd_type != CommandType::Middle) {
        TraceLog(LOG_INFO, "Tried to insert non-middle command into middle commands");
        return;
    };

    middle_commands.insert(middle_commands.begin() + relative_index, cmd);
}

void PipelineItem::deleteStartCommand() {
    start_command.reset();
}

void PipelineItem::deleteEndCommand() {
    end_command.reset();
}

void PipelineItem::swapMiddleCommands(size_t a, size_t b) {
    std::swap(middle_commands[a], middle_commands[b]);
}

void PipelineItem::deleteMiddleCommand(int relative_index) {
    middle_commands.erase(middle_commands.begin() + relative_index);
}
