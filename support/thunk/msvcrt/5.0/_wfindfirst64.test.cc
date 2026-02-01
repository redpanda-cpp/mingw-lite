#include "_wfindfirst64.h"
#include "_wfindnext64.h"

#include <catch_amalgamated.hpp>

#include <stdio.h>

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

  intptr_t handle = mingw_thunk::impl::time32__wfindfirst64(pattern, &data);

  REQUIRE(handle != -1);

  do {
    if (wcsncmp(data.name, filename, MAX_PATH) == 0)
      found = true;
  } while (mingw_thunk::impl::time32__wfindnext64(handle, &data) == 0);

  REQUIRE(errno == ENOENT);

  REQUIRE(found);
}
