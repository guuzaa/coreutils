#pragma once

#include <string_view>
#include <filesystem>
#include "word_count.hpp"

namespace wc {

class WordCounter {
public:
    static WordCount count(const std::istream& read);
    static WordCount count_file(const std::filesystem::path& path);

private:
    static WordCount count_string(std::string_view content);
};

} // namespace wc 