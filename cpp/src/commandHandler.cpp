#include "commandHandler.hpp"

#include <iostream>

// Initialize Commands (ensure this is called before using CommandHandler)
extern void initializeCommands();

// StartCommand Implementation
StartCommand::StartCommand(const std::string &cmd, const StringList& args, const int stream) {
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
    std::cout << "PipelineItem: Set StartCommand - cmd: " << cmd.getCmd() << ", args: ";
    for (const auto& arg : cmd.getArgs()) std::cout << arg << " ";
    std::cout << std::endl; // Debug
}

void PipelineItem::addMiddleCommand(const MiddleCommand& cmd) {
    middle_commands.push_back(cmd);
    std::cout << "PipelineItem: Added MiddleCommand - cmd: " << cmd.getCmd() << ", args: ";
    for (const auto& arg : cmd.getArgs()) std::cout << arg << " ";
    std::cout << std::endl; // Debug
}

void PipelineItem::setEndCommand(const EndCommand& cmd) {
    end_command = cmd;
    std::cout << "PipelineItem: Set EndCommand - cmd: " << cmd.getCmd() << ", args: ";
    for (const auto& arg : cmd.getArgs()) std::cout << arg << " ";
    std::cout << std::endl; // Debug
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

nlohmann::json PipelineItem::asJSON() const {
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
    std::cout << "CommandPipeline: Added PipelineItem with "
              << (item.getStartCommand().has_value() ? "StartCommand, " : "")
              << (!item.getMiddleCommands().empty() ? std::to_string(item.getMiddleCommands().size()) + " MiddleCommands, " : "")
              << (item.getEndCommand().has_value() ? "EndCommand" : "")
              << std::endl; // Debug
}

nlohmann::json CommandPipeline::asJSON() const {
    nlohmann::json pipeline_json = nlohmann::json::array();

    for (const auto& item : pipeline_items) {
        pipeline_json.push_back(item.asJSON());
    }

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

bool CommandHandler::parseInput(const std::string& input, std::string& errorMessage) {
    auto tokens = Tokenizer::tokenize(input);

    if (tokens.empty()) {
        errorMessage = "No command entered.";
        return false;
    }

    // Split the tokens into separate commands based on '|'
    std::vector<std::vector<std::string>> commands;
    std::vector<std::string> current_command;

    for (const auto& token : tokens) {
        if (token == "|") {
            if (current_command.empty()) {
                errorMessage = "Invalid syntax: '|' cannot start or appear consecutively.";
                return false;
            }
            commands.push_back(current_command);
            current_command.clear();
        }
        else {
            current_command.push_back(token);
        }
    }

    if (!current_command.empty()) {
        commands.push_back(current_command);
    }

    if (commands.empty()) {
        errorMessage = "No commands found.";
        return false;
    }

    // Now, for each command, validate and assign to PipelineItem
    PipelineItem currentItem;

    for (size_t cmd_idx = 0; cmd_idx < commands.size(); ++cmd_idx) {
        const auto& cmd_tokens = commands[cmd_idx];
        if (cmd_tokens.empty()) {
            errorMessage = "Invalid syntax: Empty command.";
            return false;
        }

        std::string cmd = cmd_tokens[0];
        std::vector args(cmd_tokens.begin() + 1, cmd_tokens.end());

        // Handle flags, e.g., '-p'
        if (cmd == "-p") {
            if (pipeline.isParallel()) {
                errorMessage = "Duplicate '-p' flag detected.";
                return false;
            }
            pipeline.setParallel(true);
            std::cout << "CommandHandler: Detected parallel flag '-p'" << std::endl; // Debug
            continue; // Flags are handled globally, not part of PipelineItem
        }

        // Validate command
        if (!CommandRegistry::getInstance().isCommandSupported(cmd)) {
            errorMessage = "Unsupported command: " + cmd;
            return false;
        }

        // Get command specification
        const CommandSpec* spec = CommandRegistry::getInstance().getCommandSpec(cmd);
        if (!spec) {
            errorMessage = "Command specification not found for: " + cmd;
            return false;
        }

        // Check arguments
        if (args.size() < spec->args.size()) {
            errorMessage = "Insufficient arguments for command: " + cmd;
            return false;
        }

        // Assign to stcmd, mdcmd, or edcmd based on command position and type
        if (cmd_idx == 0) {
            // StartCommand
            StartCommand startCmd(cmd, args);
            currentItem.setStartCommand(startCmd);
        }
        else if (cmd_idx == commands.size() - 1 && cmd == "FILEWRITE") {
            // If last command is FILEWRITE, set as edcmd
            EndCommand endCmd(cmd, args);
            currentItem.setEndCommand(endCmd);
        }
        else {
            // Middle command
            MiddleCommand middleCmd(cmd, args);
            currentItem.addMiddleCommand(middleCmd);
        }
    }

    // Add the PipelineItem to the pipeline
    // Check that the PipelineItem has at least StartCommand and one other command
    if (currentItem.getStartCommand().has_value() &&
        (currentItem.getEndCommand().has_value() || !currentItem.getMiddleCommands().empty())) {
        pipeline.addPipelineItem(currentItem);
    }
    else {
        errorMessage = "Incomplete pipeline. Ensure all commands have necessary arguments.";
        return false;
    }

    return true;
}

const CommandPipeline& CommandHandler::getPipeline() const {
    return pipeline;
}