#include <catch_amalgamated.hpp>

#include <errno.h>
#include <iconv.h>

using namespace std;

TEST_CASE("iconv state reset")
{
  iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
  REQUIRE(cd != (iconv_t)-1);

  char output[32];
  char *outptr = output;
  size_t outbytesleft = sizeof(output);

  size_t result = iconv(cd, nullptr, nullptr, &outptr, &outbytesleft);
  REQUIRE(result == 0);
}
