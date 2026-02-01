#include "_utime64.h"
#include "_stat64.h"

#include <catch_amalgamated.hpp>

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/utime.h>

TEST_CASE("_utime64 and _stat64")
{
  const char *filename = "test-_utime64.txt";

  SECTION("create test file")
  {
    FILE *fp = fopen(filename, "w");
    REQUIRE(fp != nullptr);
    fclose(fp);
  }

  struct __utimbuf64 times = {0x1'0000'0000, 0x1'0000'0000};

  int ret = mingw_thunk::impl::time32__utime64(filename, &times);

  REQUIRE(ret == 0);

  struct __stat64 stats;
  ret = mingw_thunk::impl::time32__stat64(filename, &stats);

  REQUIRE(ret == 0);
  REQUIRE(stats.st_atime == times.actime);
  REQUIRE(stats.st_mtime == times.modtime);
}
