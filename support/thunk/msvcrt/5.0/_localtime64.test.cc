#include "_localtime64.h"

#include <catch_amalgamated.hpp>

#include <time.h>

TEST_CASE("_localtime64")
{
  __time32_t time32 = 0x7FFF'FFFF - 24 * 60 * 60;
  __time64_t time64 = __time64_t(0x7FFF'FFFF) + 24 * 60 * 60;
  struct tm tm32 = *_localtime32(&time32);

  struct tm tm64 = *mingw_thunk::impl::time32__localtime64(&time64);

  REQUIRE(tm32.tm_year == tm64.tm_year);
  REQUIRE(tm32.tm_mon == tm64.tm_mon);
  REQUIRE(tm32.tm_mday + 2 == tm64.tm_mday);
  REQUIRE(tm32.tm_hour == tm64.tm_hour);
  REQUIRE(tm32.tm_min == tm64.tm_min);
  REQUIRE(tm32.tm_sec == tm64.tm_sec);
}
