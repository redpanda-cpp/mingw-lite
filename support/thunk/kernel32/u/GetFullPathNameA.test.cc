#include <catch_amalgamated.hpp>

#include <windows.h>

TEST_CASE("GetFullPathNameA")
{
  const char *path = "test-नमस्ते-GetFullPathNameA.txt";
  char buffer[MAX_PATH];
  char *filePart;

  DWORD result = GetFullPathNameA(path, MAX_PATH, buffer, &filePart);

  REQUIRE(result > 0);

  REQUIRE(strcmp(filePart, path) == 0);

  char cwd[MAX_PATH];
  DWORD cwdLen = GetCurrentDirectoryA(MAX_PATH, cwd);

  REQUIRE(cwdLen + 1 == filePart - buffer);

  REQUIRE(strncmp(buffer, cwd, cwdLen) == 0);
}
