#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

int main() {
  static std::string_view content = "Hello, world!\n";

  const fs::path::value_type *name =
#ifdef _WIN32
      L""
#endif
      "test__fstream_filesystem_path_value_type__‘name’.txt";

  {
    std::fstream ofs(name, std::ios::out);
    ofs << content;
  }

  {
    std::fstream ifs(fs::path(name), std::ios::in);
    std::string line;
    while (std::getline(ifs, line)) {
      std::cout << line << '\n';
    }
  }

  return 0;
}
