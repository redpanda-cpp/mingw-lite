#include <catch_amalgamated.hpp>
#include <iconv.h>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("convert from UTF-16 LE")
{
  iconv_case cases[] = {
      {"\x48\x00\x65\x00\x6C\x00\x6C\x00\x6F\x00"sv, "Hello"sv},
      {"\x60\x4F\x7D\x59"sv, "你好"sv},
  };

  test_happy_case("UTF-8", "UTF-16LE", begin(cases), end(cases));
}

TEST_CASE("convert from UTF-16 BE")
{
  iconv_case cases[] = {
      {"\x00\x48\x00\x65\x00\x6C\x00\x6C\x00\x6F"sv, "Hello"sv},
      {"\x4F\x60\x59\x7D"sv, "你好"sv},
  };

  test_happy_case("UTF-8", "UTF-16BE", begin(cases), end(cases));
}
