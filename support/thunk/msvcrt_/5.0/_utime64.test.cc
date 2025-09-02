#include <thunk/test.h>

#include <stdint.h>
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

  CLEAR_TOUCH_FLAG();
  int ret = _utime64(filename, &times);
  REQUIRE_TOUCHED();

  REQUIRE(ret == 0);

  struct __stat64 stats;

  CLEAR_TOUCH_FLAG();
  ret = _stat64(filename, &stats);
  REQUIRE_TOUCHED();

  REQUIRE(ret == 0);
  REQUIRE(stats.st_atime == times.actime);
  REQUIRE(stats.st_mtime == times.modtime);
}
