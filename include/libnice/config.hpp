#pragma once

#include <string>
#include <vector>
#include <variant>
#include <unordered_map>
#include <tl/optional.hpp>
#include "libnice/filesystem.hpp"
#include "libnice/lockfile.hpp"

namespace noice {
  class config {
  public:
    class variable {
    public:
      using value_type = std::variant<std::string, int, bool>;
      variable(std::string name, value_type value)
        : name_(std::move(name)), value_(std::move(value)) {}
      static std::string normalise_name(std::string_view sv);
      std::string normalised_name() const;
      friend std::string to_string(variable const&);
    private:
      std::string name_;
      value_type value_;
    };

    class section {
    public:
      section(std::vector<std::string> name)
        : name_(std::move(name)) {}
      static std::string normalise_name(std::vector<std::string> const& name);
      std::string normalised_name() const;
      std::string heading_line() const;
    private:
      std::vector<std::string> name_;
    };

    class line {
    public:
      line(std::string text, section sect, tl::optional<variable> var)
        : text_(std::move(text)), sect_(std::move(sect)), var_(std::move(var)) {}

      std::string normalised_variable_name() const {
        return var_.value().normalised_name();
      }

      section const& section() const { return sect_; }
      std::string_view text() const { return text_; }
      tl::optional<variable> const& variable() const { return var_; }
    private:
      std::string text_;
      config::section sect_;
      tl::optional<config::variable> var_;
    };

    config(fs::path path)
      : path_(std::move(path)), lockfile_(path) {}

    ~config();

  private:
    void read_config_file();
    line parse_line(section const& section, std::string text);
    std::vector<line> find_lines(std::vector<std::string> const& section, std::string_view variable) const;

    fs::path path_;
    lockfile lockfile_;
    std::unordered_map<std::string, std::vector<line>> lines_;
  };
}