#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include <vector>

namespace noice {
using sha1_digest = std::array<std::byte, 20>;

sha1_digest sha1(std::string_view content);
sha1_digest sha1(std::vector<std::byte> const& content);
sha1_digest hex_to_digest(std::string_view hex);
}