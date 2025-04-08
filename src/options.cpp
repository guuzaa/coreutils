#include "options.hpp"
#include "params.hpp"
#include <stdexcept>
#include <algorithm>

namespace wc {

Options OptionParser::parse(int argc, char* argv[]) {
    Options options{};
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.starts_with("--")) {
            parse_long_option(arg, options);
        } else if (arg.starts_with("-") && arg != "-") {
            parse_option(arg, options);
        } else {
            options.files.push_back(arg);
        }
    }
    
    // If help is requested, ensure no other options or files are present
    if (options.show_help) {
        if (options.show_lines || options.show_words || options.show_bytes || !options.files.empty()) {
            throw std::runtime_error("Error: -h/--help cannot be combined with other options or files.");
        }
        return options;
    }
    
    // If no options were specified (and help wasn't requested), show all counts
    if (!options.show_lines && !options.show_words && !options.show_bytes) {
        options.show_lines = true;
        options.show_words = true;
        options.show_bytes = true;
    }
    
    return options;
}

void OptionParser::parse_option(const std::string& opt, Options& options) {
    for (char c : opt.substr(1)) {
        bool found = false;
        for (const auto& param : COMMAND_PARAMS) {
            if (param.short_name == "-" + std::string(1, c)) {
                if (c == 'h') {
                    options.show_help = true;
                } else if (c == 'l') {
                    options.show_lines = true;
                } else if (c == 'w') {
                    options.show_words = true;
                } else if (c == 'c') {
                    options.show_bytes = true;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error("Invalid option: -" + std::string(1, c));
        }
    }
}

void OptionParser::parse_long_option(const std::string& opt, Options& options) {
    bool found = false;
    for (const auto& param : COMMAND_PARAMS) {
        if (opt == param.long_name) {
            if (opt == "--help") {
                options.show_help = true;
            } else if (opt == "--lines") {
                options.show_lines = true;
            } else if (opt == "--words") {
                options.show_words = true;
            } else if (opt == "--bytes") {
                options.show_bytes = true;
            }
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error("Invalid option: " + opt);
    }
}

} // namespace wc 