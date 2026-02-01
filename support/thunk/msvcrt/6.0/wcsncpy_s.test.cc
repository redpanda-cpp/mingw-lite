#include "wcsncpy_s.h"

#include <catch_amalgamated.hpp>

#include <iterator>

#include <wchar.h>

constexpr size_t rsize_max = SIZE_MAX >> 1;

TEST_CASE("wcsncpy_s")
{
  const wchar_t src[] = L"hello";
  constexpr size_t src_len = std::size(src) - 1;

  constexpr size_t N = 16;
  wchar_t dest[N];

  SECTION("normal case")
  {
    errno_t err = mingw_thunk::impl::fallback_wcsncpy_s(dest, N, src, src_len);

    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, src) == 0);
  }

  SECTION("boundary case")
  {
    dest[0] = 1;
    errno_t err =
        mingw_thunk::impl::fallback_wcsncpy_s(dest, src_len, src, src_len);
    REQUIRE(err == ERANGE);
    REQUIRE(wcscmp(dest, L"") == 0);

    err =
        mingw_thunk::impl::fallback_wcsncpy_s(dest, src_len + 1, src, src_len);
    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, src) == 0);

    err = mingw_thunk::impl::fallback_wcsncpy_s(
        dest, src_len + 1, src, src_len + 1);
    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, src) == 0);
  }

  SECTION("error: null src")
  {
    dest[0] = 1;
    errno_t err =
        mingw_thunk::impl::fallback_wcsncpy_s(dest, N, nullptr, src_len);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: null dest")
  {
    errno_t err =
        mingw_thunk::impl::fallback_wcsncpy_s(nullptr, N, src, src_len);
    REQUIRE(err == EINVAL);
  }

  SECTION("error: dest size is 0")
  {
    dest[0] = 1;
    errno_t err = mingw_thunk::impl::fallback_wcsncpy_s(dest, 0, src, src_len);
    REQUIRE(err == EINVAL);
    REQUIRE(dest[0] == 1);
  }

  SECTION("error: dest size too large")
  {
    dest[0] = 1;
    // ISO C semantic (Microsoft say OK)
    errno_t err =
        mingw_thunk::impl::fallback_wcsncpy_s(dest, rsize_max, src, src_len);
    REQUIRE(err == EINVAL);
    REQUIRE(dest[0] == 1);
  }

  SECTION("error: src count is 0")
  {
    dest[0] = 1;
    // IOS C semantic (Microsoft say OK)
    errno_t err = mingw_thunk::impl::fallback_wcsncpy_s(dest, N, src, 0);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: src count too large")
  {
    dest[0] = 1;
    // ISO C semantic (Microsoft say OK)
    errno_t err =
        mingw_thunk::impl::fallback_wcsncpy_s(dest, N, src, rsize_max);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: overlap")
  {
    dest[0] = 1;
    // ISO C semantic (Microsoft say undefined)
    errno_t err = mingw_thunk::impl::fallback_wcsncpy_s(dest, N, dest, src_len);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }
}
