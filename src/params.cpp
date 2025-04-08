#include "params.hpp"
#include <iostream>
#include <filesystem>

namespace wc {

void print_usage(const char* program_name) {
    std::string prog = std::filesystem::path(program_name).filename().string();
    std::cout << "Usage: " << prog << " [OPTION]... [FILE]...\n";
    std::cout << "Print newline, word, and byte counts for each FILE.\n\n";
    
    for (const auto& param : COMMAND_PARAMS) {
        std::cout << "  " << param.short_name << ", " << param.long_name 
                 << "\t" << param.description << "\n";
    }
    
    std::cout << "\nWith no FILE, or when FILE is -, read standard input.\n";
}
} 