#pragma once

#include <utility>
#include <string>
#include <algorithm>
#include <numeric>
#include <variant>
#include <vector>
#include <type_traits>
#include <fmt/format.h>
#include "../filesystem.hpp"
#include "../functional.hpp"
#include "../hash.hpp"

namespace noice::database
{
class tree;
std::string to_string(tree const &t);

class tree
{
  public:
    class entry
    {
      public:
        entry(fs::path path, sha1_digest oid)
            : path_(path), oid_(std::move(oid))
        {
        }

        std::string name() const
        {
            return path_.filename().string();
        }

        fs::path const &path() const
        {
            return path_;
        }

        std::string_view mode() const
        {
            bool is_executable = (fs::status(path_).permissions() & fs::perms::owner_exec) != fs::perms::none;
            return is_executable ? "100755" : "100644";
        }

        sha1_digest const &oid() const
        {
            return oid_;
        }

      private:
        fs::path path_;
        sha1_digest oid_;
    };

    /**
     * Build a tree out of a list of entries
     */
    static std::unique_ptr<tree> build(std::vector<entry> entries);

    sha1_digest const &oid() const
    {
        return oid_;
    }

    /**
     * Recursively visit this tree and its subtrees with the given callable
     */
    template <class F>
    std::invoke_result_t<F, tree &> visit(F &&f)
    {
        for (auto &[name, element] : entries_)
        {
            std::visit(overload{
                           [](entry const &) {},
                           [&](std::unique_ptr<tree> const &t) {
                               t->visit(f);
                           }},
                       element);
        }
        return f(*this);
    }

    std::string_view mode() const
    {
        return "40000";
    }

    std::string_view type() const
    {
        return "tree";
    }

    friend std::string to_string(tree const &t);
  private:
    struct passkey
    {
    };

  public:
    tree(passkey) {}

  private:
    using tree_or_entry = std::variant<entry, std::unique_ptr<tree>>;
    using entries_flatmap = std::vector<std::pair<std::string, tree_or_entry>>;

    tree() = default;

    void add_entry(std::vector<std::string> path_elements, std::string_view name, entry entry);
    friend class database;
    sha1_digest oid_;
    entries_flatmap entries_;
};

} // namespace noice