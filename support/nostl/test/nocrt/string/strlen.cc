#include <catch_amalgamated.hpp>

#include <nocrt/string.h>

TEST_CASE("string/strlen")
{
  SECTION("strlen")
  {
    REQUIRE(nocrt::strlen("") == 0);
    REQUIRE(nocrt::strlen("hello") == 5);
  }

  SECTION("strnlen")
  {
    REQUIRE(nocrt::strnlen("hello", 3) == 3);
    REQUIRE(nocrt::strnlen("hello", 6) == 5);
  }
}
