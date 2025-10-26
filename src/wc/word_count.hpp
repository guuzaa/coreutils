#pragma once

#include <algorithm>

namespace wc {

struct WordCount {
  size_t lines;
  size_t words;
  size_t chars;
  size_t bytes;
  size_t max_line_length;

  WordCount operator+(const WordCount &other) const {
    return WordCount{lines + other.lines, words + other.words,
                     chars + other.chars, bytes + other.bytes,
                     std::max(max_line_length, other.max_line_length)};
  }

  void operator+=(const WordCount &other) { *this = *this + other; }
};

} // namespace wc
