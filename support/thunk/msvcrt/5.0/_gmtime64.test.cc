#include "_gmtime64.h"

#include <catch_amalgamated.hpp>

#include <time.h>

TEST_CASE("_gmtime64")
{
  for (__time64_t time64 = 0; time64 < 0x8000'0000; time64 += 0x1000'0000) {
    __time32_t time32 = time64;
    struct tm tm32 = *_gmtime32(&time32);

    struct tm tm64 = *mingw_thunk::impl::time32__gmtime64(&time64);

    REQUIRE(tm32.tm_year == tm64.tm_year);
    REQUIRE(tm32.tm_mon == tm64.tm_mon);
    REQUIRE(tm32.tm_mday == tm64.tm_mday);
    REQUIRE(tm32.tm_hour == tm64.tm_hour);
    REQUIRE(tm32.tm_min == tm64.tm_min);
    REQUIRE(tm32.tm_sec == tm64.tm_sec);
    REQUIRE(tm32.tm_wday == tm64.tm_wday);
    REQUIRE(tm32.tm_yday == tm64.tm_yday);
  }
}
