#include "CommandLineToArgvW.h"

#include <catch_amalgamated.hpp>

#include <vector>

#include <wchar.h>

TEST_CASE("CommandLineToArgvW")
{
  const wchar_t *cmdline;
  std::vector<const wchar_t *> expected;
  int argc = 0;
  LPWSTR *argv;

  std::pair<const wchar_t *, std::vector<const wchar_t *>> cases[] = {
      {
          L"test hello world",
          {L"test", L"hello", L"world"},
      },
      {
          LR"(test "hello world")",
          {L"test", L"hello world"},
      },
      {
          LR"(test C:\path\with\backslash)",
          {L"test", LR"(C:\path\with\backslash)"},
      },
      {
          L" test leading space",
          {L"", L"test", L"leading", L"space"},
      },
      {
          L"test trailing space ",
          {L"test", L"trailing", L"space"},
      },
      {
          L"test	multiple  spaces   and/or	tabs",
          {L"test", L"multiple", L"spaces", L"and/or", L"tabs"},
      },
      // https://learn.microsoft.com/en-us/cpp/c-language/parsing-c-command-line-arguments
      {
          LR"(test "abc" d e)",
          {L"test", L"abc", L"d", L"e"},
      },
      {
          LR"(test "ab\"c" "\\" d)",
          {L"test", LR"(ab"c)", LR"(\)", L"d"},
      },
      {
          LR"(test a\\b d"e f"g h)",
          {L"test", LR"(a\\b)", L"de fg", L"h"},
      },
      {
          LR"(test a\\\"b c d)",
          {L"test", LR"(a\"b)", L"c", L"d"},
      },
      {
          LR"(test a\\\\"b c" d e)",
          {L"test", LR"(a\\b c)", L"d", L"e"},
      },
      // That page shows an insane example:
      //   LR"(test a"b"" c d)"
      // CRT gives:
      //   {L"test", LR"(ab" c d)"}
      // CommandLineToArgvW gives:
      //   {L"test", LR"(ab")", L"c", L"d"}
      // Fortunately, the standard way won't generate such a case.
      //   _Everyone quotes command line arguments the wrong way_
      //   https://learn.microsoft.com/en-us/archive/blogs/twistylittlepassagesallalike/everyone-quotes-command-line-arguments-the-wrong-way
      // So it's safe to ignore it.

      // https://devblogs.microsoft.com/oldnewthing/20100917-00/?p=12833
      {
          LR"(program.exe "hello there.txt")",
          {L"program.exe", L"hello there.txt"},
      },
      {
          LR"(program.exe "C:\Hello there.txt")",
          {L"program.exe", LR"(C:\Hello there.txt)"},
      },
      {
          LR"(program.exe "hello\"there")",
          {L"program.exe", LR"(hello"there)"},
      },
      {
          LR"(program.exe "hello\\")",
          {L"program.exe", LR"(hello\)"},
      },
  };

  for (auto &[cmdline, expected] : cases) {
    int argc = 0;
    LPWSTR *argv = mingw_thunk::impl::win9x_CommandLineToArgvW(cmdline, &argc);
    REQUIRE(argc == expected.size());
    for (int i = 0; i < argc; i++) {
      REQUIRE(wcscmp(argv[i], expected[i]) == 0);
    }
  }
}
