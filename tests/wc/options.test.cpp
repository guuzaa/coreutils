#include "gtest/gtest.h"
#include "options.hpp"
#include <vector>
#include <string>
#include <tuple>

using namespace wc;

// Test fixture for OptionParser tests
class OptionParserTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }

    // Helper function to create argv array from vector of strings
    std::vector<char*> createArgv(const std::vector<std::string>& args) {
        std::vector<char*> argv;
        for (auto& arg : args) {
            argv.push_back(const_cast<char*>(arg.c_str()));
        }
        return argv;
    }
};

TEST_F(OptionParserTest, BasicOptions) {
    struct TestCase {
        std::vector<std::string> args;
        bool show_lines;
        bool show_words;
        bool show_bytes;
        bool show_chars;
        bool show_max_line_length;
        bool show_help;
        bool show_version;
        TotalWhen total;
    };

    std::vector<TestCase> test_cases = {
        // Combined short options
        {{"program", "-lwc"}, true, true, true, false, false, false, false, TotalWhen::Auto},
        // Individual short options
        {{"program", "-l", "-w", "-c"}, true, true, true, false, false, false, false, TotalWhen::Auto},
        // Character and max line length
        {{"program", "-m", "-L"}, false, false, false, true, true, false, false, TotalWhen::Auto},
        // Help and version
        {{"program", "--help"}, false, false, false, false, false, true, false, TotalWhen::Auto},
        {{"program", "--version"}, false, false, false, false, false, false, true, TotalWhen::Auto},
        {{"program", "--help", "--version"}, false, false, false, false, false, true, true, TotalWhen::Auto},
        // Default (no arguments)
        {{"program"}, true, true, true, false, false, false, false, TotalWhen::Auto},
    };

    for (const auto& tc : test_cases) {
        auto argv = createArgv(tc.args);
        Options options = OptionParser::parse(argv.size(), argv.data());
        
        EXPECT_EQ(options.show_lines, tc.show_lines) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.show_words, tc.show_words) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.show_bytes, tc.show_bytes) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.show_chars, tc.show_chars) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.show_max_line_length, tc.show_max_line_length) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.show_help, tc.show_help) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.show_version, tc.show_version) << "Failed for args: " << tc.args[1];
        EXPECT_EQ(options.total, tc.total) << "Failed for args: " << tc.args[1];
    }
}

TEST_F(OptionParserTest, TotalOption) {
    struct TestCase {
        std::string total_arg;
        TotalWhen expected;
    };

    std::vector<TestCase> test_cases = {
        {"auto", TotalWhen::Auto},
        {"always", TotalWhen::Always},
        {"only", TotalWhen::Only},
        {"never", TotalWhen::Never},
    };

    for (const auto& tc : test_cases) {
        std::vector<std::string> args = {"program", "--total=" + tc.total_arg};
        auto argv = createArgv(args);
        Options options = OptionParser::parse(argv.size(), argv.data());
        EXPECT_EQ(options.total, tc.expected) << "Failed for --total=" << tc.total_arg;
    }
}

TEST_F(OptionParserTest, FileArguments) {
    struct TestCase {
        std::vector<std::string> args;
        std::vector<std::string> expected_files;
    };

    std::vector<TestCase> test_cases = {
        {{"program", "file1.txt", "file2.txt"}, {"file1.txt", "file2.txt"}},
        {{"program", "-l", "file1.txt", "-w", "file2.txt", "-c"}, {"file1.txt", "file2.txt"}},
        {{"program"}, {""}}, // No files
        {{"program", "single.txt"}, {"single.txt"}},
    };

    for (const auto& tc : test_cases) {
        auto argv = createArgv(tc.args);
        Inputs inputs(argv.size(), argv.data());
        auto files = inputs.array();
        EXPECT_EQ(files.size(), tc.expected_files.size()) << "Failed for args: " << tc.args[1];
        for (size_t i = 0; i < tc.expected_files.size(); ++i) {
            EXPECT_EQ(files[i].as_str(), tc.expected_files[i]) << "Failed for file index " << i;
        }
    }
}

TEST_F(OptionParserTest, IsTotalRowVisible) {
    struct TestCase {
        TotalWhen when;
        size_t num_inputs;
        bool expected;
    };

    std::vector<TestCase> test_cases = {
        // Auto: visible when more than 1 input
        {TotalWhen::Auto, 0, false},
        {TotalWhen::Auto, 1, false},
        {TotalWhen::Auto, 2, true},
        {TotalWhen::Auto, 10, true},
        
        // Always: always visible
        {TotalWhen::Always, 0, true},
        {TotalWhen::Always, 1, true},
        {TotalWhen::Always, 2, true},
        
        // Only: always visible
        {TotalWhen::Only, 0, true},
        {TotalWhen::Only, 1, true},
        {TotalWhen::Only, 2, true},
        
        // Never: never visible
        {TotalWhen::Never, 0, false},
        {TotalWhen::Never, 1, false},
        {TotalWhen::Never, 2, false},
    };

    for (const auto& tc : test_cases) {
        bool result = is_total_row_visible(tc.when, tc.num_inputs);
        EXPECT_EQ(result, tc.expected) 
            << "Failed for when=" << static_cast<int>(tc.when) 
            << ", num_inputs=" << tc.num_inputs;
    }
}

TEST_F(OptionParserTest, InputClass) {
    struct TestCase {
        std::string input;
        InputKind kind;
        bool expected_is_stdin;
        std::string expected_as_str;
    };

    std::vector<TestCase> test_cases = {
        {"", InputKind::Stdin, true, ""},
        {"test.txt", InputKind::Path, false, "test.txt"},
        {"another/file.txt", InputKind::Path, false, "another/file.txt"},
    };

    for (const auto& tc : test_cases) {
        Input input(tc.input, tc.kind);
        EXPECT_EQ(input.is_stdin(), tc.expected_is_stdin) << "Failed for input: " << tc.input;
        EXPECT_EQ(input.as_str(), tc.expected_as_str) << "Failed for input: " << tc.input;
    }
}

TEST_F(OptionParserTest, InputsClass) {
    struct TestCase {
        std::vector<std::string> args;
        size_t expected_size;
        bool first_is_stdin;
        std::string first_as_str;
    };

    std::vector<TestCase> test_cases = {
        {{"program"}, 1, true, ""}, // No args -> stdin
        {{"program", "file1.txt", "file2.txt"}, 2, false, "file1.txt"},
        {{"program", "single.txt"}, 1, false, "single.txt"},
    };

    for (const auto& tc : test_cases) {
        auto argv = createArgv(tc.args);
        Inputs inputs(argv.size(), argv.data());
        auto input_array = inputs.array();
        
        ASSERT_EQ(input_array.size(), tc.expected_size) << "Failed for args: " << tc.args[0];
        if (!input_array.empty()) {
            EXPECT_EQ(input_array[0].is_stdin(), tc.first_is_stdin) << "Failed for args: " << tc.args[0];
            EXPECT_EQ(input_array[0].as_str(), tc.first_as_str) << "Failed for args: " << tc.args[0];
        }
    }
}

TEST_F(OptionParserTest, InvalidOptions) {
    struct TestCase {
        std::vector<std::string> args;
        bool should_throw;
    };

    std::vector<TestCase> test_cases = {
        {{"program", "-x"}, true}, // Invalid short option
        {{"program", "--invalid"}, true}, // Invalid long option
        {{"program", "--invalid=value"}, true}, // Invalid long option with value
        {{"program", "-l"}, false}, // Valid short option
        {{"program", "--lines"}, false}, // Valid long option
        {{"program", "file.txt"}, false}, // Valid file argument
    };

    for (const auto& tc : test_cases) {
        auto argv = createArgv(tc.args);
        if (tc.should_throw) {
            EXPECT_THROW(OptionParser::parse(argv.size(), argv.data()), std::runtime_error)
                << "Should throw for args: " << tc.args[1];
        } else {
            EXPECT_NO_THROW(OptionParser::parse(argv.size(), argv.data()))
                << "Should not throw for args: " << tc.args[1];
        }
    }
}
