#include "commandHandler.hpp"

// StartCommand Implementation
StartCommand::StartCommand(const std::string& cmd, const StringList& args, int stream) {
    this->cmd = cmd;
    this->args = args;
    this->stream = stream;
}

CommandType StartCommand::get_type() const {
    return CommandType::Start;
}

nlohmann::json StartCommand::as_json() const {
    nlohmann::json stcmd_json;
    stcmd_json["cmd"] = cmd;
    stcmd_json["args"] = args;
    stcmd_json["stream"] = stream;
    return { {"stcmd", stcmd_json} };
}

nlohmann::json StartCommand::args_as_json() const {
    return args;
}

// MiddleCommand Implementation
MiddleCommand::MiddleCommand(const std::string& cmd, const StringList& args, int stream) {
    this->cmd = cmd;
    this->args = args;
    this->stream = stream;
}

CommandType MiddleCommand::get_type() const {
    return CommandType::Middle;
}

nlohmann::json MiddleCommand::as_json() const {
    nlohmann::json mdcmd_json;
    mdcmd_json["cmd"] = cmd;
    mdcmd_json["args"] = args;
    mdcmd_json["stream"] = stream;
    return mdcmd_json;
}

nlohmann::json MiddleCommand::args_as_json() const {
    return args;
}

// EndCommand Implementation
EndCommand::EndCommand(const std::string& cmd, const StringList& args, int stream) {
    this->cmd = cmd;
    this->args = args;
    this->stream = stream;
}

CommandType EndCommand::get_type() const {
    return CommandType::End;
}

nlohmann::json EndCommand::as_json() const {
    nlohmann::json edcmd_json;
    edcmd_json["cmd"] = cmd;
    edcmd_json["args"] = args;
    edcmd_json["stream"] = stream;
    return { {"edcmd", edcmd_json} };
}

nlohmann::json EndCommand::args_as_json() const {
    return args;
}

// PipelineItem Implementation
void PipelineItem::set_start_command(std::shared_ptr<StartCommand> cmd) {
    start_command = std::move(cmd);
}

void PipelineItem::add_middle_command(std::shared_ptr<MiddleCommand> cmd) {
    middle_commands.push_back(std::move(cmd));
}

void PipelineItem::set_end_command(std::shared_ptr<EndCommand> cmd) {
    end_command = std::move(cmd);
}

std::shared_ptr<StartCommand> PipelineItem::get_start_command() const {
    return start_command;
}

const std::vector<std::shared_ptr<MiddleCommand>>& PipelineItem::get_middle_commands() const {
    return middle_commands;
}

std::shared_ptr<EndCommand> PipelineItem::get_end_command() const {
    return end_command;
}

nlohmann::json PipelineItem::as_json() const {
    nlohmann::json json_item;

    if (start_command) {
        json_item.merge_patch(start_command->as_json());
    }

    if (!middle_commands.empty()) {
        nlohmann::json mdcmds_json = nlohmann::json::array();
        for (const auto& cmd : middle_commands) {
            mdcmds_json.push_back(cmd->as_json());
        }
        json_item["mdcmd"] = mdcmds_json;
    }

    if (end_command) {
        json_item.merge_patch(end_command->as_json());
    }

    return json_item;
}

// CommandPipeline Implementation
void CommandPipeline::add_pipeline_item(std::shared_ptr<PipelineItem> item) {
    pipeline_items.push_back(std::move(item));
}

nlohmann::json CommandPipeline::as_json() const {
    nlohmann::json pipeline_json = nlohmann::json::array();

    for (const auto& item : pipeline_items) {
        pipeline_json.push_back(item->as_json());
    }

    return { {"pipeline", pipeline_json} };
}