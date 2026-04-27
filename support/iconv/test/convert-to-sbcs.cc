#include <catch_amalgamated.hpp>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("convert to CP437")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Ænglisc"sv, "\x92nglisc"sv},
  };

  test_happy_case("cp437", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP737")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Γειά σου"sv, "\x82\x9c\xa0\xe1 \xa9\xa6\xac"sv},
  };

  test_happy_case("cp737", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP775")
{
  // missing info
}

TEST_CASE("convert to CP850")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Ænglisc"sv, "\x92nglisc"sv},
  };

  test_happy_case("cp850", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP852")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Cześć"sv, "Cze\x98\x86"sv},
  };

  test_happy_case("cp852", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP855")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Здраво"sv, "\xf4\xa6\xe1\xa0\xeb\xd6"sv},
  };

  test_happy_case("cp855", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP857")
{
  // missing info
}

TEST_CASE("convert to CP862 ")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"שלום"sv, "\x99\x8c\x85\x8d"sv},
  };

  test_happy_case("cp862", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP866")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Привет"sv, "\x8f\xe0\xa8\xa2\xa5\xe2"sv},
  };

  test_happy_case("cp866", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP874 ")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"สวัสดี"sv, "\xca\xc7\xd1\xca\xb4\xd5"sv},
  };

  test_happy_case("cp874", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1250")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Cześć"sv, "Cze\x9c\xe6"sv},
  };

  test_happy_case("cp1250", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1251")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Привет"sv, "\xcf\xf0\xe8\xe2\xe5\xf2"sv},
      {"Здраво"sv, "\xc7\xe4\xf0\xe0\xe2\xee"sv},
  };

  test_happy_case("cp1251", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1252")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Ænglisc"sv, "\xc6nglisc"sv},
  };

  test_happy_case("cp1252", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1253")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Γειά σου"sv, "\xc3\xe5\xe9\xdc \xf3\xef\xf5"sv},
  };

  test_happy_case("cp1253", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1254")
{
  // missing info
}

TEST_CASE("convert to CP1255")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"שלום"sv, "\xf9\xec\xe5\xed"sv},
  };

  test_happy_case("cp1255", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1256")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"مرحبا"sv, "\xe3\xd1\xcd\xc8\xc7"sv},
  };

  test_happy_case("cp1256", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("convert to CP1257")
{
  // missing info
}

TEST_CASE("convert to CP1258")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Xin chào"sv, "Xin ch\xe0o"sv},
  };

  test_happy_case("cp1258", "UTF-8", begin(cases), end(cases));
}
