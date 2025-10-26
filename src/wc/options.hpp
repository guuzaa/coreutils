#pragma once

#include <istream>
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

enum InputKind {
    Stdin,
    Path,
    Files0From,
};

class Input {
public:
    Input() = delete;
    Input(std::string_view input, InputKind kind): input_(input), kind_(kind) {}
    
    bool is_stdin() const {
        return this->kind_ == InputKind::Stdin;
    }
    
    std::string as_str() const {
        return input_;
    }

private:
    std::string input_;
    InputKind kind_;
};

class Inputs {
public:
    Inputs() = default;
    Inputs(int argc, char* argv[]);
    
    std::vector<Input> array() const {
        return inputs_;
    }
    
    size_t size() const {
        return inputs_.size();
    }

private:
    void parse_files0(std::istream& read);
    
    std::vector<Input> inputs_;
};

struct Options {
    bool show_lines = false;
    bool show_words = false;
    bool show_bytes = false;
    bool show_chars = false;
    bool show_max_line_length = false;
    bool show_help = false;
    bool show_version = false;
    std::optional<Input> files0_from = {};
    TotalWhen total = TotalWhen::Auto;
    
    bool no_options_enabled() const;
};

class OptionParser {
public:
    static Options parse(int argc, char* argv[]);
    static std::pair<std::string, std::string> split_long_option(const std::string& opt);
    
private:
    static void parse_option(std::string_view opt, Options& options);
    static void parse_long_option(std::string_view opt, Options& options);
};

} // namespace wc 