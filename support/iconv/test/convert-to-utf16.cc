#include <catch_amalgamated.hpp>
#include <iconv.h>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("convert to UTF-16 LE")
{
  iconv_case cases[] = {
      {"Hello"sv, "\x48\x00\x65\x00\x6C\x00\x6C\x00\x6F\x00"sv},
      {"你好"sv, "\x60\x4F\x7D\x59"sv},
  };

  test_happy_case("UTF-16LE", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to UTF-16 BE")
{
  iconv_case cases[] = {
      {"Hello"sv, "\x00\x48\x00\x65\x00\x6C\x00\x6C\x00\x6F"sv},
      {"你好"sv, "\x4F\x60\x59\x7D"sv},
  };

  test_happy_case("UTF-16BE", "UTF-8", begin(cases), end(cases));
}
