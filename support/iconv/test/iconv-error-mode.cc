#include <catch_amalgamated.hpp>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("TRANSLIT ISO-8859-1")
{
  iconv_case cases[] = {
      {"€"sv, "?"sv},
  };
  test_happy_case("ISO-8859-1//TRANSLIT", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("TRANSLIT ASCII")
{
  iconv_case cases[] = {
      {"Hello 世界"sv, "Hello ??"sv},
  };
  test_happy_case("ASCII//TRANSLIT", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("IGNORE ISO-8859-1")
{
  iconv_case cases[] = {
      {"A€B"sv, "AB"sv},
  };
  test_happy_case("ISO-8859-1//IGNORE", "UTF-8", begin(cases), end(cases));
}

TEST_CASE("IGNORE ASCII")
{
  iconv_case cases[] = {
      {"Hello 世界"sv, "Hello "sv},
  };
  test_happy_case("ASCII//IGNORE", "UTF-8", begin(cases), end(cases));
}
