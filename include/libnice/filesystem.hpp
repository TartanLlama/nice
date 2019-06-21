#pragma once

#include <filesystem>
namespace fs = std::filesystem;

#include <sys/stat.h>
#include <sys/types.h>

namespace noice
{
struct file_stat
{
    std::int32_t ctime, ctime_frac, mtime, m_time_frac,
        dev, ino, mode, uid, gid, file_size;
};

inline file_stat stat(fs::path const &path)
{
#ifdef _WIN32
    struct _stat sb;
    _wstat(path.c_str(), &sb);
#else
    struct stat sb;
    stat(path.c_str(), &sb);
#endif
    return file_stat{
        int32_t(sb.st_ctime), 0, int32_t(sb.st_mtime), 0,
        int32_t(sb.st_dev), sb.st_ino, sb.st_mode,
        sb.st_uid, sb.st_gid, sb.st_size};
} 
} // namespace noice