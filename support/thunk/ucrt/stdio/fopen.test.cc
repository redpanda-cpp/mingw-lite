#include <catch_amalgamated.hpp>

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

TEST_CASE("fopen")
{
  const char *paths[] = {
      "test-fopen-你好.txt",
      "test-fopen-こんにちは.txt",
      "test-fopen-안녕하세요.txt",
      "test-fopen-👋🌏.txt",
  };

  for (auto path : paths) {
    FILE *fp = fopen(path, "wb+");
    REQUIRE(fp);
    fclose(fp);
  }
}
