#include "options.hpp"
#include "params.hpp"
#include <stdexcept>
#include <format>
#include <iostream>
#include <fstream>

namespace wc {
    
Inputs::Inputs(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg.starts_with("--files0-from")) {
            auto [opt, value] = OptionParser::split_long_option(arg);
            if (value == "-") {
                this->parse_files0(std::cin);
            } else {
                auto file_list = std::ifstream(value);
                if (!file_list.is_open()) {
                    throw std::runtime_error(std::format("Failed to open file list: {}", value));
                }
                this->parse_files0(file_list);
            }
        } else if (!arg.starts_with('-')) {
            this->inputs_.emplace_back(arg, InputKind::Path);
        }
    }
    
    if (this->inputs_.empty()) {
        this->inputs_.emplace_back("", InputKind::Stdin);
    }
}

void Inputs::parse_files0(std::istream& read) {
    std::string line;
    while (std::getline(read, line, '\0')) {
        if (!line.empty()) {
            this->inputs_.emplace_back(line, InputKind::Path);
        }
    }
}

bool Options::no_options_enabled() const {
    return !this->show_lines && !this->show_words && !this->show_bytes && 
            !this->show_chars && !this->show_max_line_length && !this->show_version && !this->show_help;
}

Options OptionParser::parse(int argc, char* argv[]) {
    Options options{};
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg.starts_with("--")) {
            if (arg.find("=") != std::string::npos) {
                auto [opt, value] = split_long_option(arg);
                if (opt == "--total") {
                    options.total = parse_total_when_from_str(value);
                } else if (opt == "--files0-from") {
                    options.files0_from = {value, InputKind::Files0From};
                } else {
                    throw std::runtime_error(std::format("Invalid option with value: {}", opt));
                }
            } else {
                parse_long_option(arg, options);
            }
        } else if (arg.starts_with("-") && arg != "-") {
            parse_option(arg, options);
        }
    }
    
    if (options.no_options_enabled()) {
        options.show_lines = true;
        options.show_words = true;
        options.show_bytes = true;
    }
    return options;
}

std::pair<std::string, std::string> OptionParser::split_long_option(const std::string& opt) {
    size_t pos = opt.find("=");
    if (pos == std::string::npos) {
        throw std::runtime_error(std::format("Invalid option format: {}", opt));
    }
    return {opt.substr(0, pos), opt.substr(pos + 1)};
}

TotalWhen parse_total_when_from_str(std::string_view value) {
    if (value == "auto") return TotalWhen::Auto;
    if (value == "always") return TotalWhen::Always;
    if (value == "only") return TotalWhen::Only;
    if (value == "never") return TotalWhen::Never;
    throw std::runtime_error(std::format("Invalid value for --total: {}", value));
}

void OptionParser::parse_option(std::string_view opt, Options& options) {
    for (char c : opt.substr(1)) {
        bool found = false;
        for (const auto& param : COMMAND_PARAMS) {
            if (param.short_name == "-" + std::string(1, c)) {
                switch (c) {
                    case 'l':
                        options.show_lines = true;
                        break;
                    case 'w':
                        options.show_words = true;
                        break;
                    case 'c':
                        options.show_bytes = true;
                        break;
                    case 'm':
                        options.show_chars = true;
                        break;
                    case 'L':
                        options.show_max_line_length = true;
                        break;
                    case 'V':
                        options.show_version = true;
                        break;
                }
                found = true;
                break;
            }
        }
        if (!found) {
            throw std::runtime_error(std::format("Invalid option: -{}", c));
        }
    }
}

void OptionParser::parse_long_option(std::string_view opt, Options& options) {
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
            } else if (opt == "--chars") {
                options.show_chars = true;
            } else if (opt == "--max-line-length") {
                options.show_max_line_length = true;
            } else if (opt == "--version") {
                options.show_version = true;
            }
            found = true;
            break;
        }
    }
    if (!found) {
        throw std::runtime_error(std::format("Invalid option: {}", opt));
    }
}

bool is_total_row_visible(TotalWhen when, size_t num_inputs) {
    switch (when) {
        case TotalWhen::Auto:
            return num_inputs > 1;
        case TotalWhen::Always:
            return true;
        case TotalWhen::Only:
            return true;
        case TotalWhen::Never:
            return false;
    }
}

} // namespace wc 