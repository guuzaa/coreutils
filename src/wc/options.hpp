#pragma once

#include <string>
#include <vector>
#include <utility>

namespace wc {

enum class TotalWhen {
    Auto,
    Always,
    Only,
    Never
};

struct Options {
    bool show_lines = false;
    bool show_words = false;
    bool show_bytes = false;
    bool show_chars = false;
    bool show_max_line_length = false;
    bool show_help = false;
    bool show_version = false;
    std::string files0_from;
    TotalWhen total = TotalWhen::Auto;
    std::vector<std::string> files;
};

class OptionParser {
public:
    static Options parse(int argc, char* argv[]);
    
private:
    static void parse_option(const std::string& opt, Options& options);
    static void parse_long_option(const std::string& opt, Options& options);
    static TotalWhen parse_total_when(const std::string& value);
    static std::pair<std::string, std::string> split_long_option(const std::string& opt);
};

} // namespace wc 