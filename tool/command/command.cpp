#include "command.hpp"
#include "add.hpp"
#include "commit.hpp"
#include "init.hpp"
#include <iostream>
#include <memory>

namespace {
  auto init_commands() {
    std::unordered_map<std::string, std::unique_ptr<nicecli::command>> commands;
    commands.emplace("add", std::make_unique<nicecli::add>());
    commands.emplace("commit", std::make_unique<nicecli::commit>());
    commands.emplace("init", std::make_unique<nicecli::init>());
    return commands;
  }
  std::unordered_map<std::string, std::unique_ptr<nicecli::command>> s_commands = init_commands();
}

namespace nicecli {
  int run_command(ranges::span<std::string_view> args) {
    if (args[1] == "help") {
      if (args.size() == 2) {
        std::cout << "Printing help";
        return 0;
      }
      else if (args.size() > 2 && s_commands.count(std::string(args[2]))) {
        s_commands[std::string(args[2])]->help(args);
        return 0;
      }
      else {
        std::cerr << "Not recognized\n";
        return -1;
      }
    }

    else if (s_commands.count(std::string(args[1]))) {
      return s_commands[std::string(args[1])]->run(args);
    }

    std::cerr << "Not recognized\n";
    return -1;
  }
}