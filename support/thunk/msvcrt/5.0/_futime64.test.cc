#include "_futime64.h"
#include "_fstat64.h"

#include <catch_amalgamated.hpp>

#include <fcntl.h>
#include <io.h>
#include <stdint.h>
#include <sys/stat.h>
#include <sys/utime.h>

TEST_CASE("_futime64 and _fstat64")
{
  const wchar_t *filename = L"test-_futime64.txt";

  int fd = _wopen(filename, _O_CREAT | _O_WRONLY, _S_IRWXU);

  REQUIRE(fd >= 0);

  struct __utimbuf64 times = {0x1'0000'0000, 0x1'0000'0000};

  int ret = mingw_thunk::impl::time32__futime64(fd, &times);

  REQUIRE(ret == 0);

  struct __stat64 stats;

  ret = mingw_thunk::impl::time32__fstat64(fd, &stats);

  REQUIRE(ret == 0);
  REQUIRE(stats.st_mtime == times.modtime);

  // Skip: on Linux (Wine), accessing a file modified in the future will update
  // its atime unless the file system is mounted with 'noatime'.
  /* REQUIRE(stats.st_atime == times.actime); */

  close(fd);
}
