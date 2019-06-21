#pragma once

#include <fmt/format.h>
#include <tl/optional.hpp>

#include "../hash.hpp"
#include "author.hpp"
#include "database.hpp"

namespace noice::database {
/**
 * Represents a commit
 * Stores the parent commit (optional), commit oid, author, and commit message.
 */
class commit {
public:
    commit(tl::optional<sha1_digest> const& parent, sha1_digest const& tree_oid, author auth, std::string message)
      : parent_(parent), tree_oid_(tree_oid), author_(std::move(auth)), message_(std::move(message))
    {}

    std::string_view type() const {
        return "commit";
    }

    /**
     * Commits are formatted like:
     * tree <tree oid>
     * (parent <parent oid>)?
     * author <author string(see author.hpp)>
     * committer <commiter string(see author.hpp)>
     */
    friend std::string to_string(commit const& comm) {
        auto parent_string = comm.parent_ ? fmt::format("parent {}\n", bytes_to_hex(*comm.parent_)) : "";
        return fmt::format("tree {}\n{}author {}\ncommiter {}\n\n{}", 
                           bytes_to_hex(comm.tree_oid_), parent_string, to_string(comm.author_), 
                           to_string(comm.author_), comm.message_);
    }

private:
    tl::optional<sha1_digest> parent_;
    sha1_digest tree_oid_;
    author author_;
    std::string message_;
    sha1_digest oid_;
    friend class database;
};
}