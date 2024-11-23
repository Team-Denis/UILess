#pragma once

// FIXME: This file is just a weird draft for now

#include <string>
#include <nlohmann/json.hpp>

enum class CommandType {
  Begin,
  Normal,
  End,
};

class Command {
  virtual CommandType get_type() = 0;
  virtual nlohmann::json as_json() = 0;

protected:
  virtual nlohmann::json args_as_json() = 0;
};

class NormalCommand : Command {
  CommandType get_type() override {
    return CommandType::Normal;
  }

  nlohmann::json as_json() override {
      return {
          {"type", "stcmd"},
          {"args", args_as_json()},
      };
  }
};

class BeginCommand : Command {
  CommandType get_type() override {
    return CommandType::Begin;
  }
};

class EndCommand : Command {
  CommandType get_type() override {
    return CommandType::End;
  }
};

class FileToStream : BeginCommand {
  nlohmann::json args_as_json() override {
    return {
        {"path", "test"},
    };
  }
};

