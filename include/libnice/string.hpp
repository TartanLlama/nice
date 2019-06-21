#pragma once

#include <string>
#include <string_view>

namespace noice {
  std::string to_lower(std::string s);
  bool is_integer(std::string_view sv);
}