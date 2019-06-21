#include "libnice/index.hpp"
#include "libnice/filesystem.hpp"
#include "libnice/binary.hpp"
#include "libnice/hash.hpp"
#include <range/v3/algorithm/equal.hpp>
#include <iostream>
#include <fstream>

namespace
{
template <typename T>
T swap_endian(const T &val)
{
    int totalBytes = sizeof(val);
    T swapped = (T)0;
    for (int i = 0; i < totalBytes; ++i)
    {
        swapped |= (val >> (8 * (totalBytes - i - 1)) & 0xFF) << (8 * i);
    }
    return swapped;
}


struct header {
	std::array<char, 4> signature;
	std::uint32_t version_be;
	std::uint32_t count_be;
};

header read_header(std::istream& is) {
    //TODO pack
	header head;
	is.read(reinterpret_cast<char*>(&head), sizeof(header));
	if (!is) throw std::runtime_error("Index header malformed (not enough data)");

    if (std::string_view(head.signature.data(), 4) != "DIRC") {
        throw std::runtime_error("Index header malformed (bad signature)");
    }

    if (swap_endian(head.version_be) != 2) {
        throw std::runtime_error("Index header malformed (bad version)");
    }

	return head;
}

std::vector<std::byte> read_entry_data(std::istream& is) {
    constexpr auto min_entry_size = 64;
    constexpr auto block_size = 8;

    //Read 64 bytes, then keep reading 8 bytes until we find a null terminator
    std::vector<std::byte> entry_data;
    entry_data.resize(min_entry_size);
    is.read(reinterpret_cast<char*>(entry_data.data()), min_entry_size);
    if (!is) {
        throw std::runtime_error("Index malformed (bad entry)");
    }

    while (entry_data.back() != std::byte{'\0'}) {
        //not at the end of the path name, so keep reading blocks
        entry_data.resize(entry_data.size() + block_size);
        is.read(reinterpret_cast<char*>(entry_data.data() + entry_data.size() - block_size), block_size);
        if (!is) {
            throw std::runtime_error("Index malformed (bad entry)");
        } 
    }

	return entry_data;
}

std::map<fs::path, noice::index::entry> read_entries(std::istream& is) {
    std::map<fs::path, noice::index::entry> entries;
    auto head = read_header(is);
	auto head_ptr = reinterpret_cast<std::byte*>(&head);
	std::vector<std::byte> data_read (head_ptr, head_ptr + sizeof(head));
    for (int i = 0; i < swap_endian(head.count_be); ++i) {  
		auto entry_data = read_entry_data(is);
		data_read.insert(end(data_read), begin(entry_data), end(entry_data));
        auto entry = noice::index::entry::parse(entry_data);
		entries.emplace(entry.path(), entry);
    }
    noice::sha1_digest loaded_checksum;
    is.read(reinterpret_cast<char*>(&loaded_checksum), sizeof(loaded_checksum));
    auto calculated_checksum = noice::sha1(data_read);

    if (!ranges::equal(loaded_checksum, calculated_checksum)) {
        throw std::runtime_error("Index malformed (bad checksum)");
    }

    return entries;
}
} // namespace

namespace noice
{

index::entry index::entry::parse(ranges::span<std::byte> bytes) {
    file_stat big_endian_stats;
    auto pack_pointer = bytes.data();
    std::memcpy(reinterpret_cast<char*>(&big_endian_stats), pack_pointer, sizeof(big_endian_stats));
    pack_pointer += sizeof(big_endian_stats);

    //the index is big_endian, so byteswap everything
    file_stat stats{
        swap_endian(big_endian_stats.ctime), swap_endian(big_endian_stats.ctime_frac), swap_endian(big_endian_stats.mtime), swap_endian(big_endian_stats.m_time_frac),
        swap_endian(big_endian_stats.dev), swap_endian(big_endian_stats.ino), swap_endian(big_endian_stats.mode), swap_endian(big_endian_stats.uid), swap_endian(big_endian_stats.gid), swap_endian(big_endian_stats.file_size)};

    sha1_digest oid;
    std::memcpy(reinterpret_cast<char*>(oid.data()), pack_pointer, sizeof(oid));
    pack_pointer += sizeof(oid);

    std::int16_t flags;
    std::memcpy(reinterpret_cast<char*>(&flags), pack_pointer, sizeof(flags));
    pack_pointer += sizeof(flags);
    flags = swap_endian(flags);

    std::string path_name (reinterpret_cast<char*>(pack_pointer));
    return {std::move(path_name), oid, stats};
}

std::string to_string(index::entry const &e)
{
    constexpr std::size_t max_path_size = 0xfff;
    auto path_str = e.path_.lexically_normal().generic_string();
    std::int16_t flags = std::min(path_str.size(), max_path_size);
    flags = swap_endian(flags);

    auto size = sizeof(file_stat) + sizeof(sha1_digest) + sizeof(flags) + path_str.size();
    //align to 8 bytes
    auto alignment = 8;
    auto padding = alignment - (size % alignment);
	if (padding == 0) padding = 8; //there must be at least one null byte
    size += padding;
    std::vector<std::byte> data;

    //the index is big_endian, so byteswap everything
    file_stat big_endian_status{
        swap_endian(e.status_.ctime), swap_endian(e.status_.ctime_frac), swap_endian(e.status_.mtime), swap_endian(e.status_.m_time_frac),
        swap_endian(e.status_.dev), swap_endian(e.status_.ino), swap_endian(e.status_.mode), swap_endian(e.status_.uid), swap_endian(e.status_.gid), swap_endian(e.status_.file_size)};

    data.reserve(size);
    append_bytes(data, big_endian_status);
    append_bytes(data, e.oid_);
    append_bytes(data, flags);
    append_string(data, path_str);
    data.insert(end(data), padding, std::byte{0});

    return {reinterpret_cast<char*>(data.data()), data.size()};
}

void index::load_entries_from_disk() {
    std::ifstream index_file (path_, std::ios::in|std::ios::binary);
    if (index_file) {
        entries_ = read_entries(index_file); 
    }
}

index::~index()
{
	if (!need_to_update_disk_) {
		index_lock_.abort();
		return;
	}

	std::array<std::byte, 4 * 3> header;
	auto version = swap_endian(2);
	auto count = swap_endian(std::int32_t(entries_.size()));
	std::string data = "DIRC";
	data.append(reinterpret_cast<char*>(&version), 4);
	data.append(reinterpret_cast<char*>(&count), 4);
	data += ranges::accumulate(entries_, std::string{ "" }, ranges::plus{}, [](auto && e) { return to_string(e.second); });
	auto sha = sha1(data);
	data.append(std::string_view(reinterpret_cast<char*>(sha.data()), sha.size()));
	index_lock_.write(data);
}
} // namespace noice