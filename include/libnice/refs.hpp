#pragma once

#include <fstream>
#include <tl/optional.hpp>
#include "filesystem.hpp"
#include "hash.hpp"
#include "format.hpp"
#include "lockfile.hpp"

namespace noice
{
class refs
{
  public:
    refs(fs::path path)
        : path_(std::move(path))
    {
    }

    void update_head(sha1_digest oid) {
        auto lock = lockfile(head_path());
        lock.write(bytes_to_hex(oid));
        lock.write("\n");
    }

    tl::optional<std::string> read_head() {
        if (fs::exists(head_path())) {
            std::string head;
            std::getline(std::ifstream(head_path()), head);
            return head;
        }
        return tl::nullopt;
    }

    fs::path head_path() {
        return path_ / "HEAD";
    }
private : fs::path path_;
};
} // namespace noice