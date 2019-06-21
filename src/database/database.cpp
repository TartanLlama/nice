#include <fstream>
#include <sstream>
#include <iterator>
#include <iomanip>

#include "libnice/database/database.hpp"

namespace {
/**
 * Compress data using Zlib
 */
std::vector<std::byte> compress_data(std::string_view content)
{
    auto max_compressed_size = compressBound(content.size());
    std::vector<std::byte> compressed_data(max_compressed_size);
    uLongf compressed_size = compressed_data.size();
    auto ret = compress(reinterpret_cast<Bytef*>(compressed_data.data()), &compressed_size,
             reinterpret_cast<Bytef const*>(content.data()), content.size());
    compressed_data.resize(compressed_size);
    return compressed_data;
}
}

namespace noice::database {

void database::write_object(std::string_view oid, std::string_view content)
{
    //The first two characters of the SHA-1 hex string name the directory
    //The rest name the file
    auto object_dir = path_ / oid.substr(0, 2);
    fs::create_directory(object_dir);
    auto object_path = object_dir / oid.substr(2);

    if (fs::exists(object_path)) {
        return;
    }
    auto temp_file_path = fs::path("temp");
    {
        auto temp_file = std::ofstream(temp_file_path, std::ios::binary);
        auto compressed_data = compress_data(content);
        for(auto c : compressed_data) temp_file << static_cast<char>(c);
    }
    fs::rename(temp_file_path, object_path);
}
}