#include <catch_amalgamated.hpp>

#include <iconv.h>
#include <errno.h>
#include <string_view>

#include "util.h"

using namespace std;

TEST_CASE("iconv illegal sequence")
{
  error_test_case cases[] = {
      {"invalid-sequence", "UTF-16LE", "UTF-8", "\xFF\xFF"sv, 32, EILSEQ},
      {"ignore-errors", "UTF-16LE", "UTF-8",
       "\x41\x42\xFF\xFF\x43\x44"sv, 32, EILSEQ},
  };

  test_error_case(std::begin(cases), std::end(cases));
}
