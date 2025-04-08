#include <iostream>
#include <format>
#include <filesystem>
#include <sstream>
#include "wc.hpp"
#include "options.hpp"
#include "params.hpp"

std::string format_output(const wc::CountResult& result, const wc::Options& options, const std::string& filename = "") {
    std::string output;
    if (options.show_lines) {
        output += std::format("{:>8}", result.lines);
    }
    if (options.show_words) {
        output += std::format("{:>8}", result.words);
    }
    if (options.show_bytes) {
        output += std::format("{:>8}", result.bytes);
    }
    if (!filename.empty()) {
        output += " " + filename;
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
        
        if (options.files.empty()) {
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

        for (const auto& file : options.files) {
            auto result = wc::WordCounter::count_file(file);
            std::cout << format_output(result, options, file) << std::endl;

            total_lines += result.lines;
            total_words += result.words;
            total_chars += result.characters;
            total_bytes += result.bytes;
        }

        if (options.files.size() > 1) {
            wc::CountResult total{total_lines, total_words, total_chars, total_bytes};
            std::cout << format_output(total, options, "total") << std::endl;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "wc: " << e.what() << std::endl;
        return 1;
    }
}
