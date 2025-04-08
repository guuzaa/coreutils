#include "wc.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace wc {

CountResult WordCounter::count_file(const std::filesystem::path& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path.string());
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return count_string(buffer.str());
}

CountResult WordCounter::count_string(std::string_view content) {
    return count_content(content);
}

CountResult WordCounter::count_content(std::string_view content) {
    CountResult result{};
    result.bytes = content.size();
    
    bool in_word = false;
    for (char c : content) {
        result.characters++;
        
        if (c == '\n') {
            result.lines++;
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
    }
    
    return result;
}

} // namespace wc 