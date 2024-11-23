#include "commandHandler.hpp"

// StartCommand Implementation
StartCommand::StartCommand(const std::string& cmd, const StringList& args, int stream) {
    this->cmd = cmd;
    this->args = args;
    this->stream = stream;
}

CommandType StartCommand::getType() const {
    return CommandType::Start;
}

nlohmann::json StartCommand::asJSON() const {
    nlohmann::json stcmd_json;
    stcmd_json["cmd"] = cmd;
    stcmd_json["args"] = args;
    stcmd_json["stream"] = stream;
    return { {"stcmd", stcmd_json} };
}

nlohmann::json StartCommand::argsAsJSON() const {
    return args;
}

// MiddleCommand Implementation
MiddleCommand::MiddleCommand(const std::string& cmd, const StringList& args, int stream) {
    this->cmd = cmd;
    this->args = args;
    this->stream = stream;
}

CommandType MiddleCommand::getType() const {
    return CommandType::Middle;
}

nlohmann::json MiddleCommand::asJSON() const {
    nlohmann::json mdcmd_json;
    mdcmd_json["cmd"] = cmd;
    mdcmd_json["args"] = args;
    mdcmd_json["stream"] = stream;
    return mdcmd_json;
}

nlohmann::json MiddleCommand::argsAsJSON() const {
    return args;
}

// EndCommand Implementation
EndCommand::EndCommand(const std::string& cmd, const StringList& args, int stream) {
    this->cmd = cmd;
    this->args = args;
    this->stream = stream;
}

CommandType EndCommand::getType() const {
    return CommandType::End;
}

nlohmann::json EndCommand::asJSON() const {
    nlohmann::json edcmd_json;
    edcmd_json["cmd"] = cmd;
    edcmd_json["args"] = args;
    edcmd_json["stream"] = stream;
    return { {"edcmd", edcmd_json} };
}

nlohmann::json EndCommand::argsAsJSON() const {
    return args;
}

// PipelineItem Implementation
void PipelineItem::setStartCommand(const StartCommand& cmd) {
    start_command = cmd;
}

void PipelineItem::addMiddleCommand(const MiddleCommand& cmd) {
    middle_commands.push_back(cmd);
}

void PipelineItem::setEndCommand(const EndCommand& cmd) {
    end_command = cmd;
}

const std::optional<StartCommand>& PipelineItem::getStartCommand() const {
    return start_command;
}

const std::vector<MiddleCommand>& PipelineItem::getMiddleCommands() const {
    return middle_commands;
}

const std::optional<EndCommand>& PipelineItem::getEndCommand() const {
    return end_command;
}

nlohmann::json PipelineItem::as_json() const {
    nlohmann::json json_item;

    if (start_command.has_value()) {
        json_item.merge_patch(start_command->asJSON());
    }

    if (!middle_commands.empty()) {
        nlohmann::json mdcmds_json = nlohmann::json::array();
        for (const auto& cmd : middle_commands) {
            mdcmds_json.push_back(cmd.asJSON());
        }
        json_item["mdcmd"] = mdcmds_json;
    }

    if (end_command.has_value()) {
        json_item.merge_patch(end_command->asJSON());
    }

    return json_item;
}

// CommandPipeline Implementation
void CommandPipeline::addPipelineItem(const PipelineItem& item) {
    pipeline_items.push_back(item);
}

nlohmann::json CommandPipeline::as_json() const {
    nlohmann::json pipeline_json = nlohmann::json::array();

    for (const auto& item : pipeline_items) {
        pipeline_json.push_back(item.as_json());
    }

    return { {"pipeline", pipeline_json} };
}