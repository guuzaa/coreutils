#include <iostream>
#include <format>
#include <filesystem>
#include <sstream>
#include <fstream>
#include "wc.hpp"
#include "options.hpp"
#include "params.hpp"

std::string format_output(const wc::CountResult& result, const wc::Options& options, const std::string& filename = "", bool is_total = false) {
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
            output += std::format("{:>8}", result.characters);
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
            output += std::format("{:>8}", result.characters);
        }
        if (options.show_max_line_length) {
            output += std::format("{:>8}", result.max_line_length);
        }
        
        // Add filename or "total" label
        if (!filename.empty()) {
            output += " " + filename;
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
            std::cout << "wc (GNU coreutils) 0.1\n";
            return 0;
        }
        
        // Handle files0-from option
        std::vector<std::string> files_to_process = options.files;
        if (!options.files0_from.empty()) {
            std::ifstream file_list(options.files0_from);
            if (!file_list.is_open()) {
                throw std::runtime_error(std::format("Failed to open file list: {}", options.files0_from));
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

        size_t total_lines = 0;
        size_t total_words = 0;
        size_t total_chars = 0;
        size_t total_bytes = 0;
        size_t total_max_line_length = 0;

        // Process each file
        for (const auto& file : files_to_process) {
            auto result = wc::WordCounter::count_file(file);
            
            // Update totals
            total_lines += result.lines;
            total_words += result.words;
            total_chars += result.characters;
            total_bytes += result.bytes;
            total_max_line_length = std::max(total_max_line_length, result.max_line_length);
            
            // Print individual file results if not in 'only' mode
            if (options.total != wc::TotalWhen::Only) {
                std::cout << format_output(result, options, file) << std::endl;
            }
        }

        // Handle total line based on the --total option
        bool should_print_total = false;
        
        switch (options.total) {
            case wc::TotalWhen::Auto:
                should_print_total = files_to_process.size() > 1;
                break;
            case wc::TotalWhen::Always:
                should_print_total = true;
                break;
            case wc::TotalWhen::Only:
                should_print_total = true;
                break;
            case wc::TotalWhen::Never:
                should_print_total = false;
                break;
        }
        
        if (should_print_total) {
            wc::CountResult total{total_lines, total_words, total_chars, total_bytes, total_max_line_length};
            std::string total_label = (options.total == wc::TotalWhen::Only) ? "" : "total";
            std::cout << format_output(total, options, total_label, true) << std::endl;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "wc: " << e.what() << std::endl;
        return 1;
    }
}
