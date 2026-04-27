#include <catch_amalgamated.hpp>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("convert to CP932")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"ﾃｽﾄ"sv, "\xC3\xBD\xC4"sv},
      {"テスト"sv, "\x83\x65\x83\x58\x83\x67"sv},
      {"今日は"sv, "\x8D\xA1\x93\xFA\x82\xCD"sv},
  };

  test_happy_case("cp932", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP936")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"测试"sv, "\xB2\xE2\xCA\xD4"sv},
      {"你好"sv, "\xC4\xE3\xBA\xC3"sv},
  };

  test_happy_case("cp936", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP949")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"테스트"sv, "\xC5\xD7\xBD\xBA\xC6\xAE"sv},
      {"안녕하세요"sv, "\xBE\xC8\xB3\xE7\xC7\xCF\xBC\xBC\xBF\xE4"sv},
  };

  test_happy_case("cp949", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP950")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"測試"sv, "\xB4\xFA\xB8\xD5"sv},
      {"你好"sv, "\xA7\x41\xA6\x6E"sv},
  };

  test_happy_case("cp950", "UTF-8", begin(cases), end(cases));
}
