#include <catch_amalgamated.hpp>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("convert from CP932")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xc3\xbd\xc4"sv, "ﾃｽﾄ"sv},
      {"\x83\x65\x83\x58\x83\x67"sv, "テスト"sv},
      {"\x8d\xa1\x93\xfa\x82\xcd"sv, "今日は"sv},
  };

  test_happy_case("UTF-8", "cp932", begin(cases), end(cases));
}

TEST_CASE("convert from CP936")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xB2\xE2\xCA\xD4"sv, "测试"sv},
      {"\xC4\xE3\xBA\xC3"sv, "你好"sv},
  };

  test_happy_case("UTF-8", "cp936", begin(cases), end(cases));
}

TEST_CASE("convert from CP949")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xc5\xd7\xbd\xba\xc6\xae"sv, "테스트"sv},
      {"\xbe\xc8\xb3\xe7\xc7\xcf\xbc\xbc\xbf\xe4"sv, "안녕하세요"sv},
  };

  test_happy_case("UTF-8", "cp949", begin(cases), end(cases));
}

TEST_CASE("convert from CP950")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xB4\xFA\xB8\xD5"sv, "測試"sv},
      {"\xA7\x41\xA6\x6E"sv, "你好"sv},
  };

  test_happy_case("UTF-8", "cp950", begin(cases), end(cases));
}
