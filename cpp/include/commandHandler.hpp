#pragma once

#include <string>
#include <vector>
#include <optional>

#include <nlohmann/json.hpp>

// Typedefs
using StringList = std::vector<std::string>;

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
struct Command {
    Command(std::string &name, CommandType type, CommandArg &arg) {
        this->name = name;
        this->cmd_type = type;
        this->arg = arg;
    }

    [[nodiscard]] bool isComplete() const {
        switch (arg.type) {
            case CommandArgType::None:
                return true;
            case CommandArgType::Filepath:
            case CommandArgType::Text:
                return arg.value.has_value();
        }
    }

    [[nodiscard]] nlohmann::json asJSON() const;
    [[nodiscard]] nlohmann::json argsAsJSON() const;

    std::string name;
    CommandType cmd_type;
    CommandArg arg;
};

enum class PipeLineItemState {
    Start,
    Middle,
    End,
};

// PipelineItem Class
class PipelineItem {
public:
    int pushCommand(const Command &cmd);
    void insertMiddleCommand(const Command &cmd, int relative_index);
    void deleteStartCommand();
    void deleteEndCommand();
    void swapMiddleCommands(size_t a, size_t b);
    void deleteMiddleCommand(int relative_index);

    std::optional<Command>  start_command;
    std::optional<Command>  end_command;
    std::vector<Command>    middle_commands;

    [[nodiscard]] nlohmann::json asJSON() const;
private:
    PipeLineItemState       m_state = PipeLineItemState::Start;
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
    std::vector<PipelineItem>::iterator begin()     { return pipeline_items.begin(); }
    std::vector<PipelineItem>::iterator end()       { return pipeline_items.end(); }

    // Const versions
    [[nodiscard]] std::vector<PipelineItem>::const_iterator begin()     const { return pipeline_items.begin(); }
    [[nodiscard]] std::vector<PipelineItem>::const_iterator end()       const { return pipeline_items.end(); }

    // cbegin() and cend()
    [[nodiscard]] std::vector<PipelineItem>::const_iterator cbegin()    const { return pipeline_items.cbegin(); }
    [[nodiscard]] std::vector<PipelineItem>::const_iterator cend()      const { return pipeline_items.cend(); }

private:
    std::vector<PipelineItem> pipeline_items;
    bool parallel;
};

// CommandHandler Class
class CommandHandler {
public:
    CommandHandler();

    bool parseInput(const std::string &input, std::string &errorMessage);

    [[nodiscard]] const CommandPipeline &getPipeline() const;

private:
    CommandPipeline pipeline;
};