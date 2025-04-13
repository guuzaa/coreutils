#pragma once

#include <string_view>
#include <array>

namespace wc {

struct CommandParam {
    std::string_view short_name;
    std::string_view long_name;
    std::string_view description;
};

constexpr std::array<CommandParam, 9> COMMAND_PARAMS = {{
    {"-c", "--bytes", "print the byte counts"},
    {"-m", "--chars", "print the character counts"},
    {"-l", "--lines", "print the newline counts"},
    {"", "--files0-from=F", "read input from the files specified by NUL-terminated names in file F"},
    {"-L", "--max-line-length", "print the maximum display width"},
    {"-w", "--words", "print the word counts"},
    {"", "--total=WHEN", "when to print a line with total counts; WHEN can be: auto, always, only, never"},
    {"-V", "--version", "output version information and exit"},
    {"", "--help", "display this help and exit"},
}};

void print_usage(const char* program_name);

} // namespace wc 