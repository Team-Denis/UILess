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

    [[nodiscard]] virtual CommandType get_type() const = 0;
    [[nodiscard]] virtual nlohmann::json as_json() const = 0;

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

    [[nodiscard]] CommandType get_type() const override;
    [[nodiscard]] nlohmann::json as_json() const override;

protected:
    [[nodiscard]] nlohmann::json args_as_json() const override;
};

// MiddleCommand Class
class MiddleCommand : public Command {
public:
    MiddleCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType get_type() const override;
    [[nodiscard]] nlohmann::json as_json() const override;

protected:
    [[nodiscard]] nlohmann::json args_as_json() const override;
};

// EndCommand Class
class EndCommand : public Command {
public:
    EndCommand(const std::string& cmd, const StringList& args, int stream = 0);

    [[nodiscard]] CommandType get_type() const override;
    [[nodiscard]] nlohmann::json as_json() const override;

protected:
    [[nodiscard]] nlohmann::json args_as_json() const override;
};

// PipelineItem Class
class PipelineItem {
public:
    void set_start_command(std::shared_ptr<StartCommand> cmd);
    void add_middle_command(std::shared_ptr<MiddleCommand> cmd);
    void set_end_command(std::shared_ptr<EndCommand> cmd);

    [[nodiscard]] nlohmann::json as_json() const;

private:
    std::shared_ptr<StartCommand> start_command;
    std::vector<std::shared_ptr<MiddleCommand>> middle_commands;
    std::shared_ptr<EndCommand> end_command;
};

// CommandPipeline Class
class CommandPipeline {
public:
    void add_pipeline_item(std::shared_ptr<PipelineItem> item);

    [[nodiscard]] nlohmann::json as_json() const;

private:
    std::vector<std::shared_ptr<PipelineItem>> pipeline_items;
};