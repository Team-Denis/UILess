#pragma once

#include <memory>
#include <string>
#include <vector>

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

    [[nodiscard]] virtual CommandType get_type()    const = 0;
    [[nodiscard]] virtual nlohmann::json as_json()  const = 0;

protected:
    [[nodiscard]] virtual nlohmann::json args_as_json() const = 0;

protected:
    std::string cmd;
    StringList args;
    int stream = 0;
};

// StartCommand Class
class StartCommand : public Command {
public:
    StartCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType get_type()    const override;
    [[nodiscard]] nlohmann::json as_json()  const override;

protected:
    [[nodiscard]] nlohmann::json args_as_json() const override;
};

// MiddleCommand Class
class MiddleCommand : public Command {
public:
    MiddleCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType get_type()    const override;
    [[nodiscard]] nlohmann::json as_json()  const override;

protected:
    [[nodiscard]] nlohmann::json args_as_json() const override;
};

// EndCommand Class
class EndCommand : public Command {
public:
    EndCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType get_type()    const override;
    [[nodiscard]] nlohmann::json as_json()  const override;

protected:
    [[nodiscard]] nlohmann::json args_as_json() const override;
};

// PipelineItem Class
class PipelineItem {
public:
    void set_start_command  (std::shared_ptr<StartCommand>  cmd);
    void add_middle_command (std::shared_ptr<MiddleCommand> cmd);
    void set_end_command    (std::shared_ptr<EndCommand>    cmd);

    // Getter Methods
    [[nodiscard]] std::shared_ptr<StartCommand>                         get_start_command()     const;
    [[nodiscard]] const std::vector<std::shared_ptr<MiddleCommand>>&    get_middle_commands()   const;
    [[nodiscard]] std::shared_ptr<EndCommand>                           get_end_command()       const;

    [[nodiscard]] nlohmann::json as_json() const;

private:
    std::shared_ptr<StartCommand>               start_command;
    std::vector<std::shared_ptr<MiddleCommand>> middle_commands;
    std::shared_ptr<EndCommand>                 end_command;
};

// CommandPipeline Class
class CommandPipeline {
public:
    CommandPipeline() : parallel(false) {}

    void add_pipeline_item(std::shared_ptr<PipelineItem> item);

    [[nodiscard]] nlohmann::json as_json() const;

    // Parallel flag :headache:
    void set_parallel(bool is_parallel)     { parallel = is_parallel; }
    [[nodiscard]] bool is_parallel() const  { return parallel; }

    // Iterator functions for range-based for loops
    // Non-const versions
    std::vector<std::shared_ptr<PipelineItem>>::iterator begin()    { return pipeline_items.begin(); }
    std::vector<std::shared_ptr<PipelineItem>>::iterator end()      { return pipeline_items.end(); }

    // Const versions
    [[nodiscard]] std::vector<std::shared_ptr<PipelineItem>>::const_iterator begin()    const   { return pipeline_items.begin(); }
    [[nodiscard]] std::vector<std::shared_ptr<PipelineItem>>::const_iterator end()      const   { return pipeline_items.end(); }

    // cbegin() and cend()
    [[nodiscard]] std::vector<std::shared_ptr<PipelineItem>>::const_iterator cbegin()   const   { return pipeline_items.cbegin(); }
    [[nodiscard]] std::vector<std::shared_ptr<PipelineItem>>::const_iterator cend()     const   { return pipeline_items.cend(); }

private:
    std::vector<std::shared_ptr<PipelineItem>> pipeline_items;
    bool parallel;
};