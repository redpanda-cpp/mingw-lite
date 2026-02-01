#include "wcscpy_s.h"

#include <catch_amalgamated.hpp>
#include <iterator>

#include <wchar.h>

constexpr size_t rsize_max = SIZE_MAX >> 1;

TEST_CASE("wcscpy_s")
{
  const wchar_t src[] = L"hello";
  constexpr size_t src_len = std::size(src) - 1;

  constexpr size_t N = 16;
  wchar_t dest[N];

  SECTION("normal case")
  {
    errno_t err = mingw_thunk::impl::fallback_wcscpy_s(dest, N, src);

    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, src) == 0);
  }

  SECTION("boundary case")
  {
    dest[0] = 1;
    errno_t err = mingw_thunk::impl::fallback_wcscpy_s(dest, src_len, src);
    REQUIRE(err == ERANGE);
    REQUIRE(wcscmp(dest, L"") == 0);

    err = mingw_thunk::impl::fallback_wcscpy_s(dest, src_len + 1, src);
    REQUIRE(err == 0);
    REQUIRE(wcscmp(dest, src) == 0);
  }

  SECTION("error: null src")
  {
    dest[0] = 1;
    errno_t err = mingw_thunk::impl::fallback_wcscpy_s(dest, N, nullptr);
    REQUIRE(err == EINVAL);
    REQUIRE(wcscmp(dest, L"") == 0);
  }

  SECTION("error: null dest")
  {
    errno_t err = mingw_thunk::impl::fallback_wcscpy_s(nullptr, N, src);
    REQUIRE(err == EINVAL);
  }

  SECTION("error: dest size is 0")
  {
    dest[0] = 1;
    errno_t err = mingw_thunk::impl::fallback_wcscpy_s(dest, 0, src);
    REQUIRE(err == EINVAL);
    // ISO C semantic (Microsoft say dest[0] == 0)
    REQUIRE(dest[0] == 1);
  }

  SECTION("error: dest size too large")
  {
    dest[0] = 1;
    // ISO C semantic (Microsoft say OK)
    errno_t err = mingw_thunk::impl::fallback_wcscpy_s(dest, rsize_max, src);
    REQUIRE(err == EINVAL);
    REQUIRE(dest[0] == 1);
  }
}
