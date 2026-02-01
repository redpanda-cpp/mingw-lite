#include "wcscat_s.h"

#include <catch_amalgamated.hpp>

#include <iterator>

#include <string.h>
#include <wchar.h>

constexpr size_t rsize_max = SIZE_MAX >> 1;

TEST_CASE("wcscat_s")
{
  const wchar_t str1[] = L"hello";
  constexpr size_t str1_len = std::size(str1) - 1;

  const wchar_t str2[] = L"world";
  constexpr size_t str2_len = std::size(str2) - 1;

  const wchar_t *result = L"helloworld";

  constexpr size_t N = 16;
  wchar_t dest[N];

  auto fill_dest = [&]() { memcpy(dest, str1, sizeof(str1)); };

  SECTION("normal case")
  {
    fill_dest();

    errno_t err = mingw_thunk::impl::fallback_wcscat_s(dest, N, str2);

    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, result) == 0);
  }

  SECTION("boundary case")
  {
    fill_dest();
    errno_t err =
        mingw_thunk::impl::fallback_wcscat_s(dest, str1_len + str2_len, str2);
    REQUIRE(err == ERANGE);
    REQUIRE(wcscmp(dest, L"") == 0);

    fill_dest();
    err = mingw_thunk::impl::fallback_wcscat_s(
        dest, str1_len + str2_len + 1, str2);
    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, result) == 0);
  }

  SECTION("error: null src")
  {
    fill_dest();
    errno_t err = mingw_thunk::impl::fallback_wcscat_s(dest, N, nullptr);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: null dest")
  {
    errno_t err = mingw_thunk::impl::fallback_wcscat_s(nullptr, N, str2);
    REQUIRE(err == EINVAL);
  }

  SECTION("error: dest size is 0")
  {
    dest[0] = 1;
    errno_t err = mingw_thunk::impl::fallback_wcscat_s(dest, 0, str2);
    REQUIRE(err == EINVAL);
    // ISO C semantic (Microsoft say 0)
    REQUIRE(dest[0] == 1);
  }

  SECTION("error: dest size too large")
  {
    dest[0] = 1;
    // ISO C semantic (Microsoft say OK)
    errno_t err = mingw_thunk::impl::fallback_wcscat_s(dest, rsize_max, str2);
    REQUIRE(err == EINVAL);
    REQUIRE(dest[0] == 1);
  }
}
