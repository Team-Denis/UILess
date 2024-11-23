#include <algorithm>

#include "commandHandler.hpp"

// Initialize Commands (ensure this is called before using CommandHandler)
extern void initializeCommands();

// StartCommand Implementation
StartCommand::StartCommand(const std::string &cmd, const StringList& args, int stream) {
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

    size_t i = 0;
    PipelineItem currentItem;
    bool parallelFlagDetected = false;
    bool expectCommand = true; // To track if a command is expected next

    while (i < tokens.size()) {
        std::string token = tokens[i];

        // Check for parallel flag '-p'
        if (token == "-p") {
            if (parallelFlagDetected) {
                errorMessage = "Duplicate '-p' flag detected.";
                return false;
            }
            parallelFlagDetected = true;
            pipeline.setParallel(true);
            i++; // Move to next token
            continue;
        }

        // If a pipeline symbol '|' is encountered unexpectedly
        if (token == "|") {
            if (expectCommand) {
                errorMessage = "Invalid syntax: '|' cannot start or appear consecutively.";
                return false;
            }
            expectCommand = true; // Next token should be a command
            i++; // Move to next token
            continue;
        }

        // Expecting a command
        if (!expectCommand) {
            errorMessage = "Invalid syntax: Missing '|' between commands.";
            return false;
        }

        // Validate command
        if (!CommandRegistry::getInstance().isCommandSupported(token)) {
            errorMessage = "Unsupported command: " + token;
            return false;
        }

        // Get command specification
        const CommandSpec* spec = CommandRegistry::getInstance().getCommandSpec(token);
        if (!spec) {
            errorMessage = "Command specification not found for: " + token;
            return false;
        }

        // Collect arguments based on spec
        std::vector<std::string> args;
        size_t args_needed = spec->args.size();
        bool args_valid = true;

        for (size_t arg_idx = 0; arg_idx < args_needed; ++arg_idx) {
            if (i + 1 >= tokens.size()) {
                errorMessage = "Insufficient arguments for command: " + token;
                return false;
            }

            std::string arg = tokens[i + 1];

            // If the argument is a flag '-p', handle it
            if (arg == "-p") {
                if (parallelFlagDetected) {
                    errorMessage = "Duplicate '-p' flag detected.";
                    return false;
                }
                parallelFlagDetected = true;
                pipeline.setParallel(true);
                i += 2; // Consume command and flag
                args_valid = false; // Current command does not get this argument
                break;
            }

            args.push_back(arg);
            i += 1;
        }

        if (!args_valid) {
            continue; // Move to next token
        }

        i += 1; // Move past the command

        // Add command to the current pipeline item
        if (!currentItem.getStartCommand().has_value()) {
            // First command in the pipeline item
            StartCommand startCmd(token, args);
            currentItem.setStartCommand(startCmd);
        } else if (i < tokens.size() && tokens[i] != "|") {
            // If not followed by '|', it's the last command in the pipeline
            EndCommand endCmd(token, args);
            currentItem.setEndCommand(endCmd);
        } else {
            // Middle command
            MiddleCommand middleCmd(token, args);
            currentItem.addMiddleCommand(middleCmd);
        }

        expectCommand = false; // Next token should be '|' or end
    }

    // After parsing all tokens, ensure that we have a complete PipelineItem
    if (currentItem.getStartCommand().has_value() &&
        (currentItem.getEndCommand().has_value() || !currentItem.getMiddleCommands().empty())) {
        pipeline.addPipelineItem(currentItem);
    } else {
        errorMessage = "Incomplete pipeline. Ensure all commands have necessary arguments.";
        return false;
    }

    return true;
}

const CommandPipeline& CommandHandler::getPipeline() const {
    return pipeline;
}