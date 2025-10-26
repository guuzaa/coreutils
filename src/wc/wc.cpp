#include "wc.hpp"
#include "options.hpp"
#include "word_count.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <iostream>

namespace wc {
    
WordCount WordCounter::count(const Input &input) {
    if (input.is_stdin()) {
        return WordCounter::count_stream(std::cin);
    } else {
        std::ifstream file(input.as_str());
        return count_stream(file);
    }
}

WordCount WordCounter::count_stream(const std::istream& read) {
    std::stringstream buffer;
    buffer << read.rdbuf();
    return count_string(buffer.str());
}

WordCount WordCounter::count_string(std::string_view content) {
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