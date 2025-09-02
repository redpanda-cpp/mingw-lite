#include <thunk/test.h>

#include <io.h>

#include <windows.h>

TEST_CASE("_wfindfirst64 and _wfindnext64")
{
  const wchar_t *filename = L"test-_wfindfirst64.txt";

  SECTION("create test file")
  {
    FILE *fp = _wfopen(filename, L"w");
    REQUIRE(fp != nullptr);
    fclose(fp);
  }

  const wchar_t *pattern = L"test-*.txt";

  _wfinddata64_t data;
  bool found = false;

  CLEAR_TOUCH_FLAG();
  intptr_t handle = _wfindfirst64(pattern, &data);
  REQUIRE_TOUCHED();

  REQUIRE(handle != -1);

  do {
    if (wcsncmp(data.name, filename, MAX_PATH) == 0)
      found = true;
  } while ([&]() {
    CLEAR_TOUCH_FLAG();
    int ret = _wfindnext64(handle, &data);
    REQUIRE_TOUCHED();
    return ret == 0;
  }());

  REQUIRE(errno == ENOENT);

  REQUIRE(found);
}
