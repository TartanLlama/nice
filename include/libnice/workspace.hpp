#pragma once

#include <sstream>
#include <fstream>
#include <fmt/format.h>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/filter.hpp>

#include "filesystem.hpp"

namespace noice {
class workspace {
public:
    workspace (fs::path path) 
      : path_(std::move(path))
    {}

    static std::vector<fs::path> list_files(fs::path path) {
        if (fs::is_directory(path)) {
          return fs::recursive_directory_iterator(path) 
             | ranges::view::filter([](auto&& entry) { return entry.is_regular_file(); })
             | ranges::view::transform([](auto&& entry) { return entry.path(); })
             | ranges::to_vector;
        }
        else if (fs::exists(path)) {
          return { path };
        }

        throw std::runtime_error(fmt::format("pathspec {} did not match any files", path.string()));
    }

    std::vector<fs::path> files() const {
        return list_files(path_);
    }

    std::string read_file(fs::path path) const {
        std::stringstream sstr;
        std::ifstream in(path, std::ios::binary);
        sstr << in.rdbuf();
        return sstr.str();
    }

private:
    fs::path path_;
};
}