#include <catch_amalgamated.hpp>

#include <iconv.h>
#include <errno.h>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("iconv too big")
{
  error_test_case cases[] = {
      {"e2big", "UTF-16LE", "UTF-8", "Hello"sv, 4, E2BIG},
  };

  test_error_case(std::begin(cases), std::end(cases));
}
