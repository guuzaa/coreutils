#include <iostream>
#include <format>
#include <ostream>
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
        
        auto inputs = wc::Inputs(argc, argv);
        wc::WordCount totals{};
        // Process each file
        for (const auto& file : inputs.array()) {
            auto result = wc::WordCounter::count(file);
            // Update totals
            totals += result;
            
            // Print individual file results if not in 'only' mode
            if (options.total != wc::TotalWhen::Only) {
                std::cout << format_output(result, options, file.as_str()) << '\n';
            }
        }

        // Handle total line based on the --total option
        if (wc::is_total_row_visible(options.total, inputs.size())) {
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
