#include <atomic>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <vector>

std::atomic<int64_t> g;
thread_local int64_t l;

struct accumulator {
  void operator()() {
    for (int i = 0; i < 1000000; ++i) {
      l++;
      g++;
    }
    printf("l = %" PRId64 "\n", l);
  }
};

int main() {
  std::vector<std::thread> accumulators;
  for (int i = 0; i < 10; ++i)
    accumulators.emplace_back(accumulator{});

  for (auto &t : accumulators)
    t.join();
  printf("g = %" PRId64 "\n", g.load());
  return 0;
}
