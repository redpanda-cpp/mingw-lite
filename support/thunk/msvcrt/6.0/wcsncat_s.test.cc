#include "wcsncat_s.h"

#include <catch_amalgamated.hpp>

#include <iterator>

#include <string.h>
#include <wchar.h>

constexpr size_t rsize_max = SIZE_MAX >> 1;

TEST_CASE("wcsncat_s")
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

    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(dest, N, str2, str2_len);

    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, result) == 0);
  }

  SECTION("boundary case")
  {
    fill_dest();
    errno_t err = mingw_thunk::impl::fallback_wcsncat_s(
        dest, str1_len + str2_len, str2, str2_len);
    REQUIRE(err == ERANGE);
    REQUIRE(wcscmp(dest, L"") == 0);

    fill_dest();
    err = mingw_thunk::impl::fallback_wcsncat_s(
        dest, str1_len + str2_len + 1, str2, str2_len);
    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, result) == 0);

    fill_dest();
    err = mingw_thunk::impl::fallback_wcsncat_s(
        dest, str1_len + str2_len + 1, str2, str2_len + 1);
    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, result) == 0);
  }

  SECTION("error: null src")
  {
    fill_dest();
    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(dest, N, nullptr, str2_len);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: null dest")
  {
    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(nullptr, N, str2, str2_len);
    REQUIRE(err == EINVAL);
  }

  SECTION("error: dest size is 0")
  {
    dest[0] = 1;
    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(dest, 0, str2, str2_len);
    REQUIRE(err == EINVAL);
    REQUIRE(dest[0] == 1);
  }

  SECTION("error: dest size too large")
  {
    dest[0] = 1;
    // ISO C semantic (Microsoft say OK)
    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(dest, rsize_max, str2, str2_len);
    REQUIRE(err == EINVAL);
    REQUIRE(dest[0] == 1);
  }

  SECTION("error: src count is 0")
  {
    fill_dest();
    // IOS C semantic (Microsoft say OK)
    errno_t err = mingw_thunk::impl::fallback_wcsncat_s(dest, N, str2, 0);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: src count too large")
  {
    fill_dest();
    // ISO C semantic (Microsoft say OK)
    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(dest, N, str2, rsize_max);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: overlap")
  {
    fill_dest();
    // ISO C semantic (Microsoft say undefined)
    errno_t err =
        mingw_thunk::impl::fallback_wcsncat_s(dest, N, dest, str2_len);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }
}
