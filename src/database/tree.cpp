
#include <range/v3/view/transform.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/action/reverse.hpp>
#include <range/v3/algorithm/find.hpp>
#include <range/v3/to_container.hpp>

#include "libnice/database/tree.hpp"

namespace noice::database {
    std::unique_ptr<tree> tree::build(std::vector<entry> entries)
    {
        auto root = std::make_unique<tree>(passkey{});
        for (auto &e : entries)
        {
            auto path_elements = e.path() | ranges::view::transform([](auto &&path) { return path.string(); }) | ranges::to_vector;
            auto name = path_elements.back();
            path_elements.pop_back();
            ranges::reverse(path_elements);
            root->add_entry(std::move(path_elements), name, e);
        }

        return root;
    }

    std::string to_string(tree const &t)
    {
        std::string content = "";

        auto entry_contents = t.entries_ | ranges::view::transform([&](std::pair<std::string, tree::tree_or_entry> const &element) -> std::string {
                                  auto data_to_string = [&](auto &&e) {
                                      std::string_view oid_data((const char *)e.oid().data(), e.oid().size());
                                      using namespace std::literals;
                                      return fmt::format("{} {}{}{}", e.mode(), element.first, '\0', oid_data);
                                  };
                                  return std::visit(overload{
                                                        [&](std::unique_ptr<tree> const &t) {
                                                            return data_to_string(*t);
                                                        },
                                                        [&](tree::entry const &e) {
                                                            return data_to_string(e);
                                                        }},
                                                    element.second);
                              });
        return ranges::accumulate(entry_contents, std::string{""});
    }

    void tree::add_entry(std::vector<std::string> path_elements, std::string_view name, entry entry)
    {
        if (path_elements.empty())
        {
            entries_.emplace_back(name, entry);
        }
        else
        {
            auto tree_it = ranges::find(entries_, path_elements.back(), [](auto &&element) { return element.first; });
            if (tree_it == end(entries_))
            {
                entries_.emplace_back(path_elements.back(), std::make_unique<tree>(passkey{}));
                tree_it = end(entries_) - 1;
            }
            auto &&t = std::get<1>(tree_it->second);
            path_elements.pop_back();
            t->add_entry(std::move(path_elements), name, entry);
        }
    }
}