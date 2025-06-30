#include <catch_amalgamated.hpp>

#include <nostl/map.h>

#include <algorithm>
#include <random>
#include <vector>

TEST_CASE("map")
{
  SECTION("constructor")
  {
    nostl::map<int, int> m1;
    REQUIRE(m1.empty());
    REQUIRE(m1.size() == 0);
    REQUIRE(m1.begin() == m1.end());

    m1[1] = -1;
    REQUIRE(m1.size() == 1);

    nostl::map<int, int> m2(nostl::move(m1));
    REQUIRE(m2.size() == 1);
    REQUIRE(m2.begin()->first == 1);
    REQUIRE(m2.begin()->second == -1);
    REQUIRE(++m2.begin() == m2.end());

    REQUIRE(m1.empty());
    REQUIRE(m1.size() == 0);
    REQUIRE(m1.begin() == m1.end());
  }

  SECTION("operator[]")
  {
    constexpr int n = 100;
    std::vector<int> values;
    for (int i = 0; i < n; i++)
      values.push_back(i);

    nostl::map<int, int> m;

    auto test_operator_index = [&]() {
      m.clear();
      for (int i = 0; i < n; i++)
        m[values[i]] = -values[i];
      REQUIRE(m.size() == n);
      for (int i = 0; i < n; i++)
        REQUIRE(m[i] == -i);
      REQUIRE(m.size() == n);
    };

    test_operator_index();
    std::mt19937 rng;
    for (int _ = 0; _ < 10; _++) {
      std::shuffle(values.begin(), values.end(), rng);
      test_operator_index();
    }
  }
}
