#include <catch_amalgamated.hpp>

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

TEST_CASE("_open")
{
  const char *paths[] = {
      "test-_open-你好.txt",
      "test-_open-こんにちは.txt",
      "test-_open-안녕하세요.txt",
      "test-_open-👋🌏.txt",
  };

  for (auto path : paths) {
    int fd = _open(
        path, _O_BINARY | _O_CREAT | _O_RDWR | _O_TRUNC, _S_IREAD | _S_IWRITE);
    REQUIRE(fd >= 0);
    _close(fd);
  }
}
