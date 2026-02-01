#include "_wutime64.h"
#include "_wstat64.h"

#include <catch_amalgamated.hpp>

#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/utime.h>

TEST_CASE("_wutime64 and _wstat64")
{
  const wchar_t *filename = L"test-_wutime64.txt";

  SECTION("create test file")
  {
    FILE *fp = _wfopen(filename, L"w");
    REQUIRE(fp != nullptr);
    fclose(fp);
  }

  struct __utimbuf64 times = {0x1'0000'0000, 0x1'0000'0000};

  int ret = mingw_thunk::impl::time32__wutime64(filename, &times);

  REQUIRE(ret == 0);

  struct __stat64 stats;
  ret = mingw_thunk::impl::time32__wstat64(filename, &stats);

  REQUIRE(ret == 0);
  REQUIRE(stats.st_atime == times.actime);
  REQUIRE(stats.st_mtime == times.modtime);
}
