#pragma once

#include <string_view>
#include <filesystem>
#include "word_count.hpp"

namespace wc {

class WordCounter {
public:
    static WordCount count_file(const std::filesystem::path& path);
    static WordCount count_string(std::string_view content);
    
private:
    static WordCount count_content(std::string_view content);
};

} // namespace wc 