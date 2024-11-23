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

// Base Command Class
class Command {
public:
    virtual ~Command() = default;

    [[nodiscard]] virtual CommandType getType()    const = 0;
    [[nodiscard]] virtual nlohmann::json asJSON()  const = 0;
    [[nodiscard]] std::string getCmd() const { return cmd; }
    [[nodiscard]] StringList getArgs() const { return args; }

protected:
    [[nodiscard]] virtual nlohmann::json argsAsJSON() const = 0;

    std::string cmd;
    StringList args;
    int stream = 0;
};

// StartCommand Class
class StartCommand final : public Command {
public:
    StartCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType getType()    const override;
    [[nodiscard]] nlohmann::json asJSON()  const override;

protected:
    [[nodiscard]] nlohmann::json argsAsJSON() const override;
};

// MiddleCommand Class
class MiddleCommand final : public Command {
public:
    MiddleCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType getType()    const override;
    [[nodiscard]] nlohmann::json asJSON()  const override;

protected:
    [[nodiscard]] nlohmann::json argsAsJSON() const override;
};

// EndCommand Class
class EndCommand final : public Command {
public:
    EndCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType getType()    const override;
    [[nodiscard]] nlohmann::json asJSON()  const override;

protected:
    [[nodiscard]] nlohmann::json argsAsJSON() const override;
};

// PipelineItem Class
class PipelineItem {
public:
    void setStartCommand  (const StartCommand& cmd);
    void addMiddleCommand (const MiddleCommand& cmd);
    void setEndCommand    (const EndCommand& cmd);

    // Getter Methods
    [[nodiscard]] const std::optional<StartCommand>&    getStartCommand()     const;
    [[nodiscard]] const std::vector<MiddleCommand>&     getMiddleCommands()   const;
    [[nodiscard]] const std::optional<EndCommand>&      getEndCommand()       const;

    [[nodiscard]] nlohmann::json asJSON() const;

private:
    std::optional<StartCommand>     start_command;
    std::vector<MiddleCommand>      middle_commands;
    std::optional<EndCommand>       end_command;
};

// CommandPipeline Class
class CommandPipeline {
public:
    CommandPipeline() : parallel(false) {}

    void addPipelineItem(const PipelineItem& item);

    [[nodiscard]] nlohmann::json asJSON() const;

    // Parallel flag
    void setParallel(const bool is_parallel)     { parallel = is_parallel; }
    [[nodiscard]] bool isParallel() const  { return parallel; }

    // Iterator functions for range-based for loops
    // Non-const versions
    std::vector<PipelineItem>::iterator begin()    { return pipeline_items.begin(); }
    std::vector<PipelineItem>::iterator end()      { return pipeline_items.end(); }

    // Const versions
    [[nodiscard]] std::vector<PipelineItem>::const_iterator begin()    const   { return pipeline_items.begin(); }
    [[nodiscard]] std::vector<PipelineItem>::const_iterator end()      const   { return pipeline_items.end(); }

    // cbegin() and cend()
    [[nodiscard]] std::vector<PipelineItem>::const_iterator cbegin()   const   { return pipeline_items.cbegin(); }
    [[nodiscard]] std::vector<PipelineItem>::const_iterator cend()     const   { return pipeline_items.cend(); }

private:
    std::vector<PipelineItem> pipeline_items;
    bool parallel;
};

// CommandHandler Class
class CommandHandler {
public:
    CommandHandler();

    // Parses the input string and builds the pipeline if valid
    bool parseInput(const std::string& input, std::string& errorMessage);

    [[nodiscard]] const CommandPipeline& getPipeline() const;

private:
    CommandPipeline pipeline;
};