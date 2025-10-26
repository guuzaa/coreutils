#include <iostream>
#include <format>
#include <filesystem>
#include <ostream>
#include <sstream>
#include <fstream>
#include "wc.hpp"
#include "options.hpp"
#include "params.hpp"
#include "word_count.hpp"

std::string format_output(const wc::WordCount& result, const wc::Options& options, std::string_view filename = "", bool is_total = false) {
    std::string output;
    
    // For 'only' mode, don't add leading spaces
    if (options.total == wc::TotalWhen::Only && is_total) {
        if (options.show_lines) {
            output += std::format("{:>8}", result.lines);
        }
        if (options.show_words) {
            output += std::format("{:>8}", result.words);
        }
        if (options.show_bytes) {
            output += std::format("{:>8}", result.bytes);
        }
        if (options.show_chars) {
            output += std::format("{:>8}", result.chars);
        }
        if (options.show_max_line_length) {
            output += std::format("{:>8}", result.max_line_length);
        }
    } else {
        // Standard formatting with leading spaces
        if (options.show_lines) {
            output += std::format("{:>8}", result.lines);
        }
        if (options.show_words) {
            output += std::format("{:>8}", result.words);
        }
        if (options.show_bytes) {
            output += std::format("{:>8}", result.bytes);
        }
        if (options.show_chars) {
            output += std::format("{:>8}", result.chars);
        }
        if (options.show_max_line_length) {
            output += std::format("{:>8}", result.max_line_length);
        }
        
        // Add filename or "total" label
        if (!filename.empty()) {
            output += std::format(" {}", filename);
        }
    }
    
    return output;
}

int main(int argc, char* argv[]) {
    try {
        auto options = wc::OptionParser::parse(argc, argv);
        
        if (options.show_help) {
            wc::print_usage(argv[0]);
            return 0;
        }
        
        if (options.show_version) {
            std::cout << "wc 0.1\n"
                      << "Copyright (C) 2025 guuzaa.\n"
                      << "License Apache 2.0: https://www.apache.org/licenses/LICENSE-2.0\n"
                      << "This is free software: you are free to change and redistribute it.\n"
                      << "There is NO WARRANTY, to the extent permitted by law.\n\n"
                      << "Written by Jowell Young (guuzaa@outlook.com).\n";
            return 0;
        }
        
        // Handle files0-from option
        std::vector<std::string> files_to_process = options.files;
        if (options.files0_from.has_value()) {
            // TODO support stdin
            std::ifstream file_list(options.files0_from.value().as_str());
            if (!file_list.is_open()) {
                throw std::runtime_error(std::format("Failed to open file list: {}", options.files0_from.value().as_str()));
            }
            
            std::string line;
            while (std::getline(file_list, line, '\0')) {
                if (!line.empty()) {
                    files_to_process.push_back(line);
                }
            }
        }
        
        // If no files specified, read from stdin
        if (files_to_process.empty()) {
            std::stringstream buffer;
            buffer << std::cin.rdbuf();
            auto result = wc::WordCounter::count_string(buffer.str());
            std::cout << format_output(result, options) << std::endl;
            return 0;
        }

        wc::WordCount totals{};
        // Process each file
        for (const auto& file : files_to_process) {
            auto result = wc::WordCounter::count_file(file);
            // Update totals
            totals += result;
            
            // Print individual file results if not in 'only' mode
            if (options.total != wc::TotalWhen::Only) {
                std::cout << format_output(result, options, file) << '\n';
            }
        }

        // Handle total line based on the --total option
        if (wc::is_total_row_visible(options.total, files_to_process.size())) {
            std::string_view total_label = (options.total == wc::TotalWhen::Only) ? "" : "total";
            std::cout << format_output(totals, options, total_label, true) << '\n';
        }
        std::cout << std::flush;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "wc: " << e.what() << std::endl;
        return 1;
    }
}
