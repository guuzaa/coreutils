#pragma once

#include <string_view>
#include "word_count.hpp"
#include "options.hpp"

namespace wc {

class WordCounter {
public:
    static WordCount count(const Input& input);

private:
    static WordCount count_stream(const std::istream& read);
    static WordCount count_string(std::string_view content);
};

} // namespace wc 