#pragma once

#include <string>
#include <fmt/format.h>

namespace noice {
template <class Rng>
std::string bytes_to_hex(Rng &&bytes)
{
    fmt::memory_buffer hex;
    hex.reserve(bytes.size() * 2);
    for (auto c : bytes)
    {
       fmt::format_to(hex, "{:>02x}", std::to_integer<int>(c));
    }
    return to_string(hex);
}


}