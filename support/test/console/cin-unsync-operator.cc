#include <iostream>
#include <string>

#include "util.h"

int main() {
  std::ios::sync_with_stdio(false);

  std::string s;

  emu_input(L"天地玄黄\n");
  std::cin >> s;
  std::cin.get();
  std::cout << s << std::endl;

  return 0;
}
