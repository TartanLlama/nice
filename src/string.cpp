#include "libnice/string.hpp"
#include <algorithm>

namespace noice {
  std::string to_lower(std::string s) {
    std::string ret = std::forward<S>(s);
    std::transform(begin(ret), end(ret), begin(ret), [](auto c) { return std::tolower(c); });
    return ret;
  }

  bool is_integer(std::string_view sv) {
    if (sv.empty()) return false;
    auto begin = [] {
      if (sv[0] == '-') {
        return begin(sv) + 1;
      }
      return begin(sv);
    };

    return std::all_of(begin, end(sv), std::isdigit);
  }
}