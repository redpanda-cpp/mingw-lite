#include <stdio.h>

#include "util.h"

int main(void) {
  char buf[80 + 1];

  write_output(L"scanf short");
  emu_input(L"天地玄黄\n");
  scanf("%80s", buf);
  getchar();
  printf("%s\n", buf);

  return 0;
}
