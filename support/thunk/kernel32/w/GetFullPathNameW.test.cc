#include "GetFullPathNameW.h"

#include <catch_amalgamated.hpp>

#include <windows.h>

TEST_CASE("GetFullPathNameW")
{
  const wchar_t *path = L"test-GetFullPathNameW.txt";
  wchar_t buffer[MAX_PATH];
  wchar_t *filePart;

  DWORD result = mingw_thunk::impl::win9x_GetFullPathNameW(
      path, MAX_PATH, buffer, &filePart);

  REQUIRE(result > 0);

  wchar_t cwd[MAX_PATH];
  DWORD cwdLen = GetCurrentDirectoryW(MAX_PATH, cwd);

  REQUIRE(cwdLen + 1 == filePart - buffer);

  REQUIRE(wcsncmp(buffer, cwd, cwdLen) == 0);
}
