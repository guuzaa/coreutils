# Coreutils Implementation

A project to implement GNU coreutils utilities in `C++` and `Zig`. This is an ongoing effort to recreate the essential Unix/Linux command-line tools.

## Implemented Utilities

- `wc`: Word, line, and byte counting utility

## Planned Utilities

The project aims to implement all core utilities, including but not limited to:
- File Management: `ls`, `cp`, `mv`, `rm`, `mkdir`, `cat`, etc.
- Text Processing: `head`, `tail`, `sort`, `uniq`, etc.
- System Information: `pwd`, `whoami`, `uname`, etc.
- And many more...

## Installation

### Prerequisites

- Zig compiler (0.14 recommended)

### Building from Source

Clone the repository and build using Zig's build system:

```bash
git clone https://github.com/guuzaa/wc.git
cd wc
zig build
```

The executables will be available in `zig-out/bin/`.

## Usage

### wc Examples

Count lines, words, and characters in a file:
```bash
wc file.txt
```

Count only lines in multiple files:
```bash
wc -l file1.txt file2.txt
```

Count words from standard input:
```bash
cat file.txt | wc -w
```

## Contributing

Contributions are welcome! Feel free to:
- Implement new utilities
- Improve existing implementations
- Report bugs
- Suggest enhancements

## License

Licensed under Apache-2.0 license ([LICENSE](LICENSE) or http://opensource.org/licenses/Apache-2.0)