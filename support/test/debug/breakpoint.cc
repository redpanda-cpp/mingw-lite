#include <stdio.h>
#include <vector>

enum {
  MAX = 5,
};

int main(void) {
  constexpr int MAX = 5;
  int fib[MAX + 1] = {0, 1};

  for (int i = 2; i <= MAX; i++) {
    fib[i] = fib[i - 1] + fib[i - 2];
    printf("%d\n", fib[i]);
  }

  std::vector<int> fib_vec(fib, fib + MAX + 1);

  return 0;
}
