#include <iostream>
#include <string>

#include "util.h"

int main() {
  std::string s;

  emu_input(L"天地玄黄\n");
  std::cin >> s;
  std::cin.get();
  std::cout << s << std::endl;

  return 0;
}
