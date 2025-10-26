#include "params.hpp"
#include <iostream>
#include <string>

namespace wc {

void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [OPTION]... [FILE]...\n"
              << "  or:  " << program_name << " [OPTION]... --files0-from=F\n"
              << "Print newline, word, and byte counts for each FILE, and a total line if\n"
              << "more than one FILE is specified.  A word is a non-zero-length sequence of\n"
              << "printable characters delimited by white space.\n\n"
              << "With no FILE, or when FILE is -, read standard input.\n\n"
              << "The options below may be used to select which counts are printed, always in\n"
              << "the following order: newline, word, character, byte, maximum line length.\n";
    
    // Print options in the correct order
    for (const auto& param : COMMAND_PARAMS) {
        // Format the option string with proper alignment
        std::string opt_str;
        if (!param.short_name.empty() && !param.long_name.empty()) {
            opt_str = std::string(param.short_name) + ", " + std::string(param.long_name);
        } else if (!param.long_name.empty()) {
            opt_str = std::format("    {}", param.long_name);
        } else {
            continue; // Skip if both short and long names are empty
        }
        
        // Print the option with proper alignment
        std::cout << "  " << opt_str;
        
        // Add padding for alignment
        size_t padding = 30 - opt_str.length();
        if (padding > 0) {
            std::cout << std::string(padding, ' ');
        }
        
        std::cout << param.description << "\n";
    }
}
} 