#pragma once

#include "command.hpp"
#include "libnice/workspace.hpp"
#include "libnice/database/blob.hpp"
#include "libnice/database/database.hpp"
#include "libnice/database/tree.hpp"
#include "libnice/database/author.hpp"
#include "libnice/database/commit.hpp"
#include "libnice/filesystem.hpp"
#include "libnice/index.hpp"
#include "libnice/refs.hpp"

#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/action/join.hpp>

#include <iostream>

namespace nicecli {
  struct add final : command {
    virtual void help(ranges::span<std::string_view> args) {
      std::cout << "Add files to the index\n";
    }
    virtual int run(ranges::span<std::string_view> args) {
      auto root_path = fs::current_path();
      auto git_path = root_path / ".git";

      auto workspace = noice::workspace(root_path);
      auto database = noice::database::database(git_path / "objects");
      auto index = noice::index(git_path / "index");

      try {
        auto paths = args
          | ranges::view::drop(2)
          | ranges::view::transform(
            [](auto&& arg_path) { return noice::workspace::list_files(fs::path(arg_path));  }
          )
          | ranges::action::join;

        for (auto&& file_path : paths) {
          auto data = workspace.read_file(file_path);
          auto stat = noice::stat(file_path);

          auto blob = noice::database::blob(data);
          database.store(blob);
          index.add(file_path, blob.oid(), stat);
        }
      }
      catch (std::runtime_error& err) {
        std::cerr << "fatal: " << err.what();
        index.release_lock();
        return 128;
      }
    }
  };
}