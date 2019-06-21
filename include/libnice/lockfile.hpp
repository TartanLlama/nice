#pragma once

#include <cstdio>
#include "filesystem.hpp"

class lockfile {
public:
    lockfile(fs::path path, bool binary_mode = false)
      : file_path_(std::move(path)), lock_path_(file_path_)
    {
        lock_path_ += ".lock";
        auto mode = binary_mode ? "wxb" : "wx"; 
        lock_ = std::fopen(lock_path_.string().c_str(), mode);
        if (!lock_) {
            throw std::runtime_error("Could not open lockfile");
        }
    }

    lockfile(lockfile const&) = delete;
    lockfile& operator=(lockfile const&) = delete;
	void abort() {
    if (lock_) {
      std::fclose(lock_);
      fs::remove(lock_path_);
      lock_ = nullptr;
    }
	}
    ~lockfile() {
		if (lock_) {
			std::fclose(lock_);
			fs::rename(lock_path_, file_path_);
		}
    }

    void write(std::string_view sv) {
        std::fwrite(sv.data(), 1, sv.size(), lock_);
    }

    template<std::size_t N>
    void write(std::array<std::byte,N> const& bytes) {
        std::fwrite(begin(bytes), 1, N, lock_);
    }

private:
    fs::path file_path_;
    fs::path lock_path_;
    std::FILE* lock_;
};