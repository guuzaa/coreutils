#include <iostream>
#include <string>

int main(int argc, char *argv[]) {
  std::string output = "";
  if (argc > 1) {
    for (int i = 1; i < argc; ++i) {
      output += argv[i];
      if (i < argc - 1) {
        output += " ";
      }
    }
  } else {
      output = "y";
  }
  while (true) {
    std::cout << output << '\n';
  }
  return 0;
}
