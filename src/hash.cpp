#include <openssl/sha.h>
#include <cassert>
#include <charconv>

#include "libnice/hash.hpp"

namespace noice {
sha1_digest sha1(std::string_view content) {
    sha1_digest oid;
    SHA1(reinterpret_cast<unsigned char const*>(content.data()), content.size(), 
         reinterpret_cast<unsigned char*>(oid.data()));
    return oid;
}

sha1_digest sha1(std::vector<std::byte> const& content) {
    sha1_digest oid;
    SHA1(reinterpret_cast<unsigned char const*>(content.data()), content.size(), 
         reinterpret_cast<unsigned char*>(oid.data()));
    return oid;
}

sha1_digest hex_to_digest(std::string_view hex) {
    sha1_digest digest;
    //SHA1 digests are 40 hex characters
    assert(hex.size() == 40);

    auto to_byte = [](std::string_view hex_chars) {
        int int_rep;
        std::from_chars(hex_chars.data(), hex_chars.data()+2, int_rep, 16);
        return static_cast<std::byte>(int_rep);
    };

    for (auto i = 0; i < 40; i+=2) {
        digest[i/2] = to_byte(hex.substr(i,2));
    }
    return digest;
}
}