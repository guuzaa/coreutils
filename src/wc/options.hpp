#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>
#include <utility>

namespace wc {

enum class TotalWhen {
    Auto,
    Always,
    Only,
    Never
};
bool is_total_row_visible(TotalWhen when, size_t num_inputs);
TotalWhen parse_total_when_from_str(std::string_view value);

class Input {
public:
    Input() = delete;
    Input(std::string_view files0): files0_from_(files0) {}
    
    bool is_stdin() {
        return this->files0_from_ == "-";
    }
    
    std::string as_str() {
        return files0_from_;
    }

private:
    std::string files0_from_;
};

struct Options {
    bool show_lines = true;
    bool show_words = true;
    bool show_bytes = true;
    bool show_chars = false;
    bool show_max_line_length = false;
    bool show_help = false;
    bool show_version = false;
    std::optional<Input> files0_from = {};
    TotalWhen total = TotalWhen::Auto;
    std::vector<std::string> files;
};

class OptionParser {
public:
    static Options parse(int argc, char* argv[]);
    
private:
    static void parse_option(std::string_view opt, Options& options);
    static void parse_long_option(std::string_view opt, Options& options);
    static std::pair<std::string, std::string> split_long_option(const std::string& opt);
};

} // namespace wc 