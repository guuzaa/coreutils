#pragma once

#include <string_view>
#include <filesystem>

namespace wc {

struct CountResult {
    size_t lines;
    size_t words;
    size_t characters;
    size_t bytes;
};

class WordCounter {
public:
    static CountResult count_file(const std::filesystem::path& path);
    static CountResult count_string(std::string_view content);
    
private:
    static CountResult count_content(std::string_view content);
};

} // namespace wc 