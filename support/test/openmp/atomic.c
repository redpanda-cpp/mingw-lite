#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

_Atomic int64_t g;

int main() {
#pragma omp parallel for num_threads(10)
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 1000 * 1000; ++j)
      g++;
  }
  printf("g = %" PRId64 "\n", g);
}
