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

    [[nodiscard]] CommandArgType getArgType() const {
        return m_arg.type;
    }

    bool isComplete() {
        switch (m_arg.type) {
            case CommandArgType::None:
                return true;
            case CommandArgType::Filepath:
            case CommandArgType::Text:
                return m_arg.value.has_value();
        }
    }

    std::string &getName() {
        return m_name;
    }

    [[nodiscard]] nlohmann::json asJSON() const;

private:
    [[nodiscard]] nlohmann::json argsAsJSON() const;

    std::string m_name;
    CommandType m_type;
    CommandArg m_arg;
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
    std::optional<Command>  m_start_command;
    std::optional<Command>  m_end_command;
    std::vector<Command>    m_middle_commands;
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