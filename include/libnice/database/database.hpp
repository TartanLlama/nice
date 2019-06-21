#pragma once

#include <fmt/format.h>
#include <zlib.h>
#include <cstdio>
#include <string>
#include <array>
#include <string_view>

#include "blob.hpp"
#include "../filesystem.hpp"
#include "../hash.hpp"
#include "../format.hpp"

namespace noice::database
{

class database
{
  public:
    database(fs::path path)
        : path_(std::move(path))
    {
    }

    /**
     * Store the given object in the database, return its object id
     * Requires object to support to_string(object) and .type()
     */
    template <class Obj>
    sha1_digest store(Obj &object)
    {
        auto to_store = to_string(object);
        // Objects in git are all written in this format
        auto content = fmt::format("{} {}{}{}", object.type(), to_store.size(), '\0', to_store);
        auto oid = sha1(content);
        object.oid_ = oid; //TODO this is awful, do this some other way
        write_object(bytes_to_hex(oid), content);
        return oid;
    }

  private:
    void write_object(std::string_view oid, std::string_view content);
    fs::path path_;
};
} // namespace noice