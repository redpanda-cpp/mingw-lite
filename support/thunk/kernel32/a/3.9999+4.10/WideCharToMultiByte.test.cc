#include "WideCharToMultiByte.h"

#include <catch_amalgamated.hpp>

#include <string.h>
#include <wchar.h>

#include <windows.h>

TEST_CASE("WideCharToMultiByte")
{
  constexpr int buffer_len = 1024;

  SECTION("ASCII")
  {
    const wchar_t *in = L"Hello, world!";
    const char *expected = "Hello, world!";
    char out[buffer_len] = {};

    int len = mingw_thunk::impl::win95_WideCharToMultiByte(
        CP_UTF8, 0, in, -1, out, buffer_len, nullptr, nullptr);
    REQUIRE(len == strlen(expected) + 1);
    REQUIRE(strcmp(out, expected) == 0);
  }

  SECTION("Get length")
  {
    const wchar_t *in = L"Hello, world!";
    const char *expected = "Hello, world!";
    char out[buffer_len] = {};

    int len = mingw_thunk::impl::win95_WideCharToMultiByte(
        CP_UTF8, 0, in, -1, nullptr, 0, nullptr, nullptr);
    REQUIRE(len == strlen(expected) + 1);

    int len2 = mingw_thunk::impl::win95_WideCharToMultiByte(
        CP_UTF8, 0, in, wcslen(in), nullptr, 0, nullptr, nullptr);
    REQUIRE(len2 == strlen(expected));
  }

  SECTION("Insufficient buffer")
  {
    const wchar_t *in = L"Hello, world!";
    const char *expected = "Hello, world!";
    char out[buffer_len] = {};

    int buffer_len_2 = 5;
    int len = mingw_thunk::impl::win95_WideCharToMultiByte(
        CP_UTF8, 0, in, -1, out, buffer_len_2, nullptr, nullptr);
    REQUIRE(len == 0);
    REQUIRE(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    REQUIRE(out[buffer_len_2] == 0);
  }

  SECTION("Normal")
  {
    const wchar_t *in = L"你好，“世界”！";
    const char *expected = "你好，“世界”！";
    char out[buffer_len] = {};

    int len = mingw_thunk::impl::win95_WideCharToMultiByte(
        CP_UTF8, 0, in, -1, out, buffer_len, nullptr, nullptr);
    REQUIRE(len == strlen(expected) + 1);
    REQUIRE(strcmp(out, expected) == 0);
  }

  SECTION("Surrogate pairs")
  {
    const wchar_t *in = L"👋🌏🌍🌎";
    const char *expected = "👋🌏🌍🌎";
    char out[buffer_len] = {};

    int len = mingw_thunk::impl::win95_WideCharToMultiByte(
        CP_UTF8, 0, in, -1, out, buffer_len, nullptr, nullptr);
    REQUIRE(len == strlen(expected) + 1);
    REQUIRE(strcmp(out, expected) == 0);
  }
}
