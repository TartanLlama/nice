#pragma once

#include <vector>
#include <cstddef>

namespace noice {
template <class T>
void append_bytes(std::vector<std::byte>& bytes, T const& t) {
    auto begin = reinterpret_cast<const std::byte*>(&t);
    bytes.insert(end(bytes), begin, begin + sizeof(t));
}

void append_string(std::vector<std::byte>& bytes, std::string_view sv) {
    auto begin = reinterpret_cast<const std::byte*>(sv.data());
    bytes.insert(end(bytes), begin, begin + sv.size());
}
}