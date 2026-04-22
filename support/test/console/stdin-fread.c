#include <stdio.h>

#include "util.h"

int main(void) {
  char buf[80 + 1];

  emu_input(L"天地玄黄\n");
  size_t n = fread(buf, 1, 13, stdin);
  buf[n] = '\0';
  fwrite(buf, 1, n, stdout);
  return 0;
}
