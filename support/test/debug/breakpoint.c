#include <stdio.h>

enum {
  MAX = 5,
};

int main(void) {
  int fib[MAX + 1] = {0, 1};

  for (int i = 2; i <= MAX; i++) {
    fib[i] = fib[i - 1] + fib[i - 2];
    printf("%d\n", fib[i]);
  }

  return 0;
}
