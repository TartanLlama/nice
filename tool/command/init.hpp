#pragma once

#include "command.hpp"
#include <iostream>

namespace nicecli {
  struct init final : command {
    virtual void help(ranges::span<std::string_view> args) {
      std::cout << "Initialize empty repository";
    }

    virtual int run(ranges::span<std::string_view> args) {
      auto root = args.size() > 2 ? fs::path(args[2]) / ".git" : fs::path(".git");
      try
      {
        fs::create_directories(root / "objects");
        fs::create_directories(root / "refs");
      }
      catch (fs::filesystem_error& err)
      {
        std::cerr << "nice: Error - " << err.what();
        return 1;
      }
      fmt::print("Initialized empty git repository in {}\n", root.string());
    }
  };
}