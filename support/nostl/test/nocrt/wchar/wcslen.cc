#include <catch_amalgamated.hpp>

#include <nocrt/wchar.h>

TEST_CASE("wchar/wcslen")
{
  SECTION("wcslen")
  {
    REQUIRE(nocrt::wcslen(L"") == 0);
    REQUIRE(nocrt::wcslen(L"hello") == 5);
  }

  SECTION("wcsnlen")
  {
    REQUIRE(nocrt::wcsnlen(L"hello", 3) == 3);
    REQUIRE(nocrt::wcsnlen(L"hello", 6) == 5);
  }
}
