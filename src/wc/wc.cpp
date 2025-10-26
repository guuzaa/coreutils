#include "wc.hpp"
#include "word_count.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <format>

namespace wc {

WordCount WordCounter::count_file(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error(std::format("Failed to open file: {}", path.string()));
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return count_string(buffer.str());
}

WordCount WordCounter::count_string(std::string_view content) {
    return count_content(content);
}

WordCount WordCounter::count_content(std::string_view content) {
    WordCount result{};
    result.bytes = content.size();
    result.max_line_length = 0;
    
    bool in_word = false;
    size_t current_line_length = 0;
    
    for (char c : content) {
        result.chars++;
        
        if (c == '\n') {
            result.lines++;
            result.max_line_length = std::max(result.max_line_length, current_line_length);
            current_line_length = 0;
        } else {
            current_line_length++;
        }
        
        if (std::isspace(c)) {
            in_word = false;
        } else if (!in_word) {
            in_word = true;
            result.words++;
        }
    }
    
    // Count the last line if it doesn't end with a newline
    if (!content.empty() && content.back() != '\n') {
        result.lines++;
        result.max_line_length = std::max(result.max_line_length, current_line_length);
    }
    
    return result;
}

} // namespace wc 