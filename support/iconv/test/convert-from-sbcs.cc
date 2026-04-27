#include <catch_amalgamated.hpp>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("convert from CP437")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\x92nglisc"sv, "Ænglisc"sv},
  };

  test_happy_case("UTF-8", "cp437", begin(cases), end(cases));
}

TEST_CASE("convert from CP737")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\x82\x9c\xa0\xe1 \xa9\xa6\xac"sv, "Γειά σου"sv},
  };

  test_happy_case("UTF-8", "cp737", begin(cases), end(cases));
}

TEST_CASE("convert from CP775")
{
  // missing info
}

TEST_CASE("convert from CP850")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\x92nglisc"sv, "Ænglisc"sv},
  };

  test_happy_case("UTF-8", "cp850", begin(cases), end(cases));
}

TEST_CASE("convert from CP852")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Cze\x98\x86"sv, "Cześć"sv},
  };

  test_happy_case("UTF-8", "cp852", begin(cases), end(cases));
}

TEST_CASE("convert from CP855")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xf4\xa6\xe1\xa0\xeb\xd6"sv, "Здраво"sv},
  };

  test_happy_case("UTF-8", "cp855", begin(cases), end(cases));
}

TEST_CASE("convert from CP857")
{
  // missing info
}

TEST_CASE("convert from CP862")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\x99\x8c\x85\x8d"sv, "שלום"sv},
  };

  test_happy_case("UTF-8", "cp862", begin(cases), end(cases));
}

TEST_CASE("convert from CP866")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\x8f\xe0\xa8\xa2\xa5\xe2"sv, "Привет"sv},
  };

  test_happy_case("UTF-8", "cp866", begin(cases), end(cases));
}

TEST_CASE("convert from CP874")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xca\xc7\xd1\xca\xb4\xd5"sv, "สวัสดี"sv},
  };

  test_happy_case("UTF-8", "cp874", begin(cases), end(cases));
}

TEST_CASE("convert from CP1250")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Cze\x9c\xe6"sv, "Cześć"sv},
  };

  test_happy_case("UTF-8", "cp1250", begin(cases), end(cases));
}

TEST_CASE("convert from CP1251")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xcf\xf0\xe8\xe2\xe5\xf2"sv, "Привет"sv},
      {"\xc7\xe4\xf0\xe0\xe2\xee"sv, "Здраво"sv},
  };

  test_happy_case("UTF-8", "cp1251", begin(cases), end(cases));
}

TEST_CASE("convert from CP1252")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xc6nglisc"sv, "Ænglisc"sv},
  };

  test_happy_case("UTF-8", "cp1252", begin(cases), end(cases));
}

TEST_CASE("convert from CP1253")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xc3\xe5\xe9\xdc \xf3\xef\xf5"sv, "Γειά σου"sv},
  };

  test_happy_case("UTF-8", "cp1253", begin(cases), end(cases));
}

TEST_CASE("convert from CP1254")
{
  // missing info
}

TEST_CASE("convert from CP1255")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xf9\xec\xe5\xed"sv, "שלום"sv},
  };

  test_happy_case("UTF-8", "cp1255", begin(cases), end(cases));
}

TEST_CASE("convert from CP1256")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"\xe3\xd1\xcd\xc8\xc7"sv, "مرحبا"sv},
  };

  test_happy_case("UTF-8", "cp1256", begin(cases), end(cases));
}

TEST_CASE("convert from CP1257")
{
  // missing info
}

TEST_CASE("convert from CP1258")
{
  iconv_case cases[] = {
      {"Hello"sv, "Hello"sv},
      {"Xin ch\xe0o"sv, "Xin chào"sv},
  };

  test_happy_case("UTF-8", "cp1258", begin(cases), end(cases));
}
