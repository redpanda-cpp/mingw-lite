#include <thunk/test.h>

#include <string.h>
#include <wchar.h>

#include <windows.h>

TEST_CASE("MultiByteToWideChar")
{
  constexpr int buffer_len = 1024;

  SECTION("ASCII")
  {
    const char *in = "Hello, world!";
    const wchar_t *expected = L"Hello, world!";
    wchar_t out[buffer_len] = {};

    CLEAR_TOUCH_FLAG();
    int len = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, buffer_len);
    REQUIRE_TOUCHED();
    REQUIRE(len == wcslen(expected) + 1);
    REQUIRE(wcscmp(out, expected) == 0);
  }

  SECTION("Get length")
  {
    const char *in = "Hello, world!";
    const wchar_t *expected = L"Hello, world!";
    wchar_t out[buffer_len] = {};

    int len = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, 0);
    REQUIRE(len == wcslen(expected) + 1);

    int len2 = MultiByteToWideChar(CP_UTF8, 0, in, strlen(in), nullptr, 0);
    REQUIRE(len2 == wcslen(expected));
  }

  SECTION("Insufficient buffer")
  {
    const char *in = "Hello, world!";
    const wchar_t *expected = L"Hello, world!";
    wchar_t out[buffer_len] = {};

    int buffer_len_2 = 5;
    int len = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, buffer_len_2);
    REQUIRE(len == 0);
    REQUIRE(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
    REQUIRE(out[buffer_len_2] == 0);
  }

  SECTION("Normal")
  {
    const char *in = "你好，“世界”！";
    const wchar_t *expected = L"你好，“世界”！";
    wchar_t out[buffer_len] = {};

    int len = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, buffer_len);
    REQUIRE(len == wcslen(expected) + 1);
    REQUIRE(wcscmp(out, expected) == 0);
  }

  SECTION("Surrogate pairs")
  {
    const char *in = "👋🌏🌍🌎";
    const wchar_t *expected = L"👋🌏🌍🌎";
    wchar_t out[buffer_len] = {};

    int len = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, buffer_len);
    REQUIRE(len == wcslen(expected) + 1);
    REQUIRE(wcscmp(out, expected) == 0);
  }
}
