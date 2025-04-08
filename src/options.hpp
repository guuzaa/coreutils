#pragma once

#include <string>
#include <vector>

namespace wc {

struct Options {
    bool show_lines = false;
    bool show_words = false;
    bool show_bytes = false;
    bool show_help = false;
    std::vector<std::string> files;
};

class OptionParser {
public:
    static Options parse(int argc, char* argv[]);
    
private:
    static void parse_option(const std::string& opt, Options& options);
    static void parse_long_option(const std::string& opt, Options& options);
};

} // namespace wc 