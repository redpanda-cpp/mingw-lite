#include "_findfirst64.h"
#include "_findnext64.h"

#include <catch_amalgamated.hpp>

#include <io.h>

#include <windows.h>

TEST_CASE("_findfirst64 and _findnext64")
{
  const char *filename = "test-_findfirst64.txt";

  SECTION("create test file")
  {
    FILE *fp = fopen(filename, "w");
    REQUIRE(fp != nullptr);
    fclose(fp);
  }

  const char *pattern = "test-*.txt";

  __finddata64_t data;
  bool found = false;

  intptr_t handle = mingw_thunk::impl::time32__findfirst64(pattern, &data);

  REQUIRE(handle != -1);

  do {
    if (strncmp(data.name, filename, MAX_PATH) == 0)
      found = true;
  } while ([&]() {
    int ret = mingw_thunk::impl::time32__findnext64(handle, &data);
    return ret == 0;
  }());

  REQUIRE(errno == ENOENT);

  REQUIRE(found);
}
