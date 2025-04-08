#pragma once

#include <string_view>
#include <array>

namespace wc {

struct CommandParam {
    std::string_view short_name;
    std::string_view long_name;
    std::string_view description;
};

constexpr std::array<CommandParam, 4> COMMAND_PARAMS = {{
    {"-l", "--lines", "print the newline counts"},
    {"-w", "--words", "print the word counts"},
    {"-c", "--bytes", "print the byte counts"},
    {"-h", "--help", "display this help and exit"}
}};

void print_usage(const char* program_name);

} // namespace wc 