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
#include <iostream>

namespace nicecli {
  struct commit final : command {
    virtual void help(ranges::span<std::string_view> args) {
      std::cout << "Commit changes\n";
    }

    virtual int run(ranges::span<std::string_view> args) {
      auto root = std::filesystem::current_path();
      auto git_root = root / ".git";
      auto db_path = git_root / "objects";

      auto database = noice::database::database(db_path);
      auto index = noice::index(git_root / "index");
      auto refs = noice::refs(git_root);

      auto index_entries = index.entries();
      auto tree_entries = index_entries
        | ranges::view::transform([](noice::index::entry const& entry) -> noice::database::tree::entry { return noice::database::tree::entry{ entry.path(), entry.oid() }; })
        | ranges::to_vector;

      auto tree = noice::database::tree::build(tree_entries);
      auto tree_oid = tree->visit([&](auto&& t) { return database.store(t); });

      auto parent = refs.read_head();
      auto name = std::getenv("GIT_AUTHOR_NAME");
      auto email = std::getenv("GIT_AUTHOR_EMAIL");
      if (!name or !email)
      {
        std::cerr << "nice: Error - GIT_AUTHOR_NAME and GIT_AUTHOR_EMAIL must be set";
        return 1;
      }
      auto time = std::time(nullptr);

      auto author = noice::database::author(name, email, time);
      std::string message;
      std::getline(std::cin, message);
      noice::database::commit commit(parent.map(noice::hex_to_digest), tree_oid, author, message);
      auto commit_oid = database.store(commit);
      refs.update_head(commit_oid);

      auto commit_oid_hex = noice::bytes_to_hex(commit_oid);

      std::ofstream head(git_root / "HEAD");
      head << commit_oid_hex;
      std::istringstream message_stream(message);
      std::string first_message_line;
      std::getline(message_stream, first_message_line);
      auto root_text = parent ? "" : "(root-commit) ";
      fmt::print("[{}{}] {}\n", root_text, commit_oid_hex, first_message_line);
    }
  };
}