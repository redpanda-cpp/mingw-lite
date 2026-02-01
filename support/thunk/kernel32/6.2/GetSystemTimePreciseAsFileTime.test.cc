#include "GetSystemTimePreciseAsFileTime.h"

#include <catch_amalgamated.hpp>

#include <windows.h>

TEST_CASE("GetSystemTimePreciseAsFileTime")
{
  FILETIME ft1;
  mingw_thunk::impl::fallback_GetSystemTimePreciseAsFileTime(&ft1);

  FILETIME ft2;
  GetSystemTimeAsFileTime(&ft2);

  ULARGE_INTEGER ul1{
      .LowPart = ft1.dwLowDateTime,
      .HighPart = ft1.dwHighDateTime,
  };
  ULARGE_INTEGER ul2{
      .LowPart = ft2.dwLowDateTime,
      .HighPart = ft2.dwHighDateTime,
  };

  // difference < 1 ms (100 ns * 10'000)
  REQUIRE(int64_t(ul1.QuadPart) - int64_t(ul2.QuadPart) < 10'000);
  REQUIRE(int64_t(ul2.QuadPart) - int64_t(ul1.QuadPart) < 10'000);
}
