#include "commandSpec.hpp"

void initializeCommands() {
    CommandRegistry& registry = CommandRegistry::getInstance();

    registry.registerCommand(CommandSpec{
            "cat",
            {ArgType::File}
    });

    registry.registerCommand(CommandSpec{
            "ls",
            {ArgType::Folder}
    });

    registry.registerCommand(CommandSpec{
            "rm",
            {ArgType::File}
    });

    registry.registerCommand(CommandSpec{
            "mkdir",
            {ArgType::Folder}
    });

    // cat filename | grep pattern

    registry.registerCommand(CommandSpec{
            "grep",
            {ArgType::String}
    });

    registry.registerCommand(CommandSpec{
            "cd",
            {ArgType::Folder}
    });

    registry.registerCommand(CommandSpec{
            "ifconfig",
            {ArgType::None}
    });

    registry.registerCommand(CommandSpec{
            "ping",
            {ArgType::String}
    });

    registry.registerCommand(CommandSpec{
            "touch",
            {ArgType::File}
    });

    registry.registerCommand(CommandSpec{
            "wc",
            {ArgType::File}
    });

    registry.registerCommand(CommandSpec{
            "neofetch",
            {ArgType::None}
    });

    registry.registerCommand(CommandSpec{
            "ps",
            {ArgType::None}
    });

    registry.registerCommand(CommandSpec{
            "FILEWRITE",
            {ArgType::File}
    });
}