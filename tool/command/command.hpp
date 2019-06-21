#pragma once

#include <range/v3/span.hpp>
#include <string_view>
#include <string>

namespace nicecli {
  struct command {
    virtual void help(ranges::span<std::string_view> args) = 0;
    virtual int run(ranges::span<std::string_view> args) = 0;
  };

    int run_command(ranges::span<std::string_view> args);
}