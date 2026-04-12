#include <iostream>
#include <source_location>

int main() {
  std::source_location loc = std::source_location::current();
  std::cout << loc.file_name() << '\n';
}
