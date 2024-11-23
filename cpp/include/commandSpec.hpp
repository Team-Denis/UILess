#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

enum class ArgType {
    None,
    File,
    Folder,
    String,
    Flag
};

struct CommandSpec {
    std::string name;
    std::vector<ArgType> args;
};

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }

    void registerCommand(const CommandSpec& spec) {
        registry.emplace(spec.name, spec);
    }

    bool isCommandSupported(const std::string& cmd) const {
        return registry.find(cmd) != registry.end();
    }

    const CommandSpec* getCommandSpec(const std::string& cmd) const {
        auto it = registry.find(cmd);

        if (it != registry.end()) {
            return &it->second;
        }
        return nullptr;
    }

    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;

private:
    CommandRegistry() = default;
    std::unordered_map<std::string, CommandSpec> registry;
};