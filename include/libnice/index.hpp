#pragma once

#include <vector>
#include <map>
#include <unordered_map>

#include <range/v3/numeric/accumulate.hpp>
#include <range/v3/span.hpp>
#include <range/v3/view/transform.hpp>
#include "database/tree.hpp"
#include "hash.hpp"
#include "lockfile.hpp"
#include "filesystem.hpp"
#include "filesystem/parents_of.hpp"
#include "functional.hpp"

namespace noice
{
class index
{
public:
  class entry
  {
  public:
    entry(fs::path path, sha1_digest const &oid, file_stat const &status)
        : path_(std::move(path)), oid_(oid), status_(status)
    {
      //Git only uses two modes, so fix up the mode
      bool is_executable = (fs::status(path_).permissions() & fs::perms::owner_exec) != fs::perms::none;
      status_.mode = is_executable ? 0100755 : 0100644;
    }

    static entry parse(ranges::span<std::byte> bytes);

    friend std::string to_string(entry const &e);

	fs::path const& path() const { return path_; }
	sha1_digest const& oid() const { return oid_;  }
	file_stat const& status() const { return status_;  }

  private:
    fs::path path_;
    sha1_digest oid_;
    file_stat status_;
  };

  index(fs::path path)
      : path_(std::move(path)), index_lock_(path_, true)
  {
    load_entries_from_disk();
  }

  void remove_entry(fs::path const& path) {
	  entries_.erase(path);
	  for (auto&& parent : parents_of(path)) {
		  auto recursive_parents = parents_.find(parent.string());
		  erase(recursive_parents->second, path);
		  if (recursive_parents->second.empty()) {
			  parents_.erase(recursive_parents);
		  }
	  }
  }

  void discard_conflicts(fs::path const& path) {
	  for (auto&& parent : parents_of(path)) {
		  remove_entry(parent);
	  }   
	  if (parents_.count(path.string())) {
		  auto path_parents = parents_[path.string()];
		  for (auto&& dir : path_parents) {
			  remove_entry(dir);
		  }
	  }
  }

  void add(fs::path const &path, sha1_digest const &oid, file_stat const &status)
  {
    discard_conflicts(path);
    entries_.insert_or_assign(path, entry{path, oid, status});
	for (auto&& parent : parents_of(path)) {
		parents_[parent.string()].push_back(path);
	}
	need_to_update_disk_ = true;
  }

  std::vector<entry> entries() const {
	  return entries_ 
		   | ranges::view::transform([](auto && pair) { return pair.second; })
		   | ranges::to_vector;
  }

  void release_lock() {
    index_lock_.abort();
  }

  index(index const &) = delete;
  index &operator=(index const &) = delete;
  ~index();

private:
  void load_entries_from_disk();

  std::map<fs::path, entry> entries_;
  std::unordered_map<std::string, std::vector<fs::path>> parents_;
  fs::path path_;
  bool need_to_update_disk_ = false;
  lockfile index_lock_;
};
} // namespace noice
