#include <thunk/test.h>

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

  CLEAR_TOUCH_FLAG();
  intptr_t handle = _findfirst64(pattern, &data);
  REQUIRE_TOUCHED();

  REQUIRE(handle != -1);

  do {
    if (strncmp(data.name, filename, MAX_PATH) == 0)
      found = true;
  } while ([&]() {
    CLEAR_TOUCH_FLAG();
    int ret = _findnext64(handle, &data);
    REQUIRE_TOUCHED();
    return ret == 0;
  }());

  REQUIRE(errno == ENOENT);

  REQUIRE(found);
}
