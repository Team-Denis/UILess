#pragma once

#include <string>
#include <vector>
#include <optional>

#include "commandSpec.hpp"
#include "tokenizer.hpp"

#include <nlohmann/json.hpp>

// Typedefs
using StringList = std::vector<std::string>;

// Enum for Command Types
enum class CommandType {
    Start,
    Middle,
    End,
};

enum class CommandArgType {
    None,
    Filepath,
    Text,
};

struct CommandArg {
    CommandArgType type = CommandArgType::None;
    std::optional<std::string> value{};
};

// Base Command Class
class Command {
public:
    Command(std::string &name, CommandType type, CommandArg &arg) {
        m_name = name;
        m_type = type;
        m_arg = arg;
    }

    [[nodiscard]] CommandType getType() const {
        return m_type;
    }

    std::string &getName() {
        return m_name;
    }

    [[nodiscard]] nlohmann::json asJSON() const;

private:
    [[nodiscard]] nlohmann::json argsAsJSON() const;

    [[nodiscard]] nlohmann::json typeAsJSON() const;

    std::string m_name;
    CommandType m_type;
    CommandArg m_arg;
};

// PipelineItem Class
class PipelineItem {
public:
    int pushCommand(const Command &cmd);

    // Getter Methods
// PipelineItem Implementation
    [[nodiscard]] const std::optional<Command> &getStartCommand() const {
        return m_start_command;
    }

    [[nodiscard]] const std::vector<Command> &getMiddleCommands() const {
        return m_middle_commands;
    }

    [[nodiscard]] const std::optional<Command> &getEndCommand() const {
        return m_end_command;
    }

    [[nodiscard]] nlohmann::json asJSON() const;

private:
    std::optional<Command> m_start_command;
    std::optional<Command> m_end_command;
    std::vector<Command> m_middle_commands;
    PipeLineItemState m_state = PipeLineItemState::Start;
};

// CommandPipeline Class
class CommandPipeline {
public:
    CommandPipeline() : parallel(false) {}

    void addPipelineItem(const PipelineItem &item);

    [[nodiscard]] nlohmann::json asJSON() const;

    // Parallel flag
    void setParallel(bool is_parallel) { parallel = is_parallel; }

    [[nodiscard]] bool isParallel() const { return parallel; }

    // Iterator functions for range-based for loops
    // Non-const versions
    std::vector<PipelineItem>::iterator begin() { return pipeline_items.begin(); }

    std::vector<PipelineItem>::iterator end() { return pipeline_items.end(); }

    // Const versions
    [[nodiscard]] std::vector<PipelineItem>::const_iterator begin() const { return pipeline_items.begin(); }

    [[nodiscard]] std::vector<PipelineItem>::const_iterator end() const { return pipeline_items.end(); }

    // cbegin() and cend()
    [[nodiscard]] std::vector<PipelineItem>::const_iterator cbegin() const { return pipeline_items.cbegin(); }

    [[nodiscard]] std::vector<PipelineItem>::const_iterator cend() const { return pipeline_items.cend(); }

private:
    std::vector<PipelineItem> pipeline_items;
    bool parallel;
};

// CommandHandler Class
class CommandHandler {
public:
    CommandHandler();

    // Parses the input string and builds the pipeline if valid
    bool parseInput(const std::string &input, std::string &errorMessage);

    [[nodiscard]] const CommandPipeline &getPipeline() const;

private:
    CommandPipeline pipeline;
};