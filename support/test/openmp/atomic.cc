#include <atomic>
#include <cinttypes>
#include <cstdint>
#include <cstdio>

std::atomic<int64_t> g;

int main() {
#pragma omp parallel for num_threads(10)
  for (int i = 0; i < 10; ++i) {
    for (int j = 0; j < 1'000'000; ++j)
      g++;
  }
  printf("g = %" PRId64 "\n", g.load());
}
