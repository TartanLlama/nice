#include "libnice/config.hpp"

#include <range/v3/to_container.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/view/join.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/filter.hpp>
#include <range/v3/numeric/accumulate.hpp>
#include <fmt/format.h>
#include <ctre.hpp>
#include <ctll.hpp>
#include <vector>
#include <fstream>
#include <string>
#include "libnice/string.hpp"
#include "libnice/functional.hpp"

namespace {
  static constexpr auto section_line_pattern = ctll::fixed_string{ R"-(^\s*\[([\-a-zA-Z0-9]+)(?: "(. + )" )?\]\s*($|#|;))-" };
  static constexpr auto variable_line_pattern = ctll::fixed_string{ "^\\s*([a-zA-Z][\\-a-zA-Z0-9]*)\\s*=\\s*(.*?)\\s*($|#|;)" };
  static constexpr auto blank_line_pattern = ctll::fixed_string{ "^\\s*($|#|;)" };


  noice::config::variable::value_type parse_value(std::string s) {
    if (s == "yes" or s == "on" or s == "true") return true;
    if (s == "no" or s == "off" or s == "false") return false;
    if (noice::is_integer(s)) return std::stoi(s);
    return std::move(s);
  }
}

namespace noice {
  std::string config::variable::normalise_name(std::string_view sv) {
    return noice::to_lower(std::string(sv));
  }

  std::string config::variable::normalised_name() const {
    return normalise_name(name_);
  }

  std::string to_string(const config::variable& var) {
    return std::visit([&](auto&& value) { return fmt::format("\t{} = {}\n", var.name_, value); }, var.value_);
  }

  std::string config::section::normalise_name(std::vector<std::string> const& name)  {
    if (name.empty()) return "";
    auto subsections = name | ranges::view::drop(1) | ranges::view::join('.') | ranges::to_<std::string>();
    return noice::to_lower(name[0]) + '.' + subsections;
  }

  std::string config::section::normalised_name() const {
    return normalise_name(name_);
  }

  std::string config::section::heading_line() const {
    std::string text = "[";
    text += name_.front();
    if (name_.size() > 1) {
      text += fmt::format(" \"{}\"", name_ | ranges::view::drop(1) | ranges::view::join('.') | ranges::to_<std::string>());
    }
    text += "]\n";
    return text;
  }

  config::line config::parse_line(section const& section, std::string text) {
    if (auto match = ctre::match<section_line_pattern>(text)) {
      auto sect = [&] {
        if (match.get<2>()) {
          return noice::config::section({ match.get<1>().to_string(), match.get<2>().to_string() });
        }
        return noice::config::section({ match.get<1>().to_string() });
      }();

      return { std::move(text), std::move(sect), tl::nullopt };
    }
    else if (auto match = ctre::match<variable_line_pattern>(text)) {
      auto value = parse_value(match.get<2>().to_string());
      variable var(match.get<1>().to_string(), value);
      return { std::move(text), section, std::move(var) };
    }
    else if (auto match = ctre::match<blank_line_pattern>(text)) {
      return { std::move(text), section, tl::nullopt };
    }
    else {
      auto line_number = ranges::accumulate(lines_, 0, ranges::plus{}, curry([](auto&&, auto&& lines) { return lines.size(); }));
      throw std::runtime_error(fmt::format("bad config line {} in file {}", line_number, path_.string()));
    }
  }

  void config::read_config_file() {
    section global_section({});

    std::ifstream file(path_);
    for (std::string line_text; std::getline(file, line_text); ) {
      auto l = parse_line(global_section, line_text);
      auto sect = l.section();

      lines_[sect.normalised_name()].push_back(std::move(l));
    }
  }


  std::vector<config::line> config::find_lines(std::vector<std::string> const &sect_name, std::string_view var_name) const {
    auto normal_sect = section::normalise_name(sect_name);
    if (!lines_.count(normal_sect)) return {};
    
    auto const& lines = lines_.at(normal_sect);
    auto const& sect = lines[0].section();
    auto normal_var = variable::normalise_name(var_name);

    return lines 
      | ranges::view::filter([&](auto&& line) { return line.normalised_variable_name() == normal_var; }) 
      | ranges::to_vector;
  }

  config::~config() {
    for (auto const& [_, lines] : lines_) {
      for (auto const& line : lines) {
        lockfile_.write(line.text());
      }
    }
  }
}