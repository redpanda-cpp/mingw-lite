#include <stdio.h>

#include "util.h"

int main(void) {
  char buf[80 + 1];

  emu_input(L"天地玄黄\n");
  fgets(buf, sizeof(buf), stdin);
  fputs(buf, stdout);

  return 0;
}
