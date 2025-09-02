#include <catch_amalgamated.hpp>

#include <nostl/__algorithm/lower_bound.h>
#include <nostl/__iterator/begin.h>
#include <nostl/__iterator/end.h>

TEST_CASE("lower_bound")
{
  SECTION("sequencial numbers")
  {
    constexpr int N = 1000;
    int a[N] = {};
    for (int i = 0; i < N; ++i) {
      a[i] = i;
    }

    // boundary case
    auto b = nostl::begin(a);
    auto e = nostl::end(a);
    REQUIRE(nostl::lower_bound(b, e, N) == e);

    auto x = GENERATE(take(100, random(0, N - 1)));
    auto it = nostl::lower_bound(b, e, x);
    REQUIRE(it == &a[x]);
    REQUIRE(*it == x);
  }

  SECTION("even in odd")
  {
    constexpr int N = 1000;
    int a[N] = {};
    for (int i = 0; i < N; ++i) {
      a[i] = i * 2;
    }

    auto b = nostl::begin(a);
    auto e = nostl::end(a);

    auto x = GENERATE(take(100, random(1, 2 * N - 1)));
    auto it = nostl::lower_bound(b, e, x);
    REQUIRE(*it >= x);
    REQUIRE(*--it < x);
  }
}
