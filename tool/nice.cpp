#include <range/v3/span.hpp>
#include <range/v3/view/drop.hpp>
#include <range/v3/view/transform.hpp>
#include <range/v3/action/join.hpp>
#include <fmt/format.h>
#include <iostream>
#include <string_view>
#include <ctime>
#include <sstream>
#include <algorithm>

#include "command/command.hpp"

void print_error(std::string_view err)
{
    std::cerr << "nice: Error - " << err << '\n';
}

int main(int argc, const char **argv)
{
    auto args = ranges::span(argv, argc) | ranges::view::transform([](auto str) { return std::string_view{str}; }) | ranges::to_vector;

	try {
    return nicecli::run_command(args);
	}
	catch (std::exception & e) {
		print_error(e.what());
		return -1;
	}    
}