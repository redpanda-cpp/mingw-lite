#include <catch_amalgamated.hpp>

#include <iconv.h>

using namespace std;

TEST_CASE("iconv return value - all reversible")
{
  iconv_t cd = iconv_open("UTF-16LE", "UTF-8");
  REQUIRE(cd != (iconv_t)-1);

  const char *input = "AB";
  size_t inbytesleft = 2;
  char output[32];
  char *outptr = output;
  size_t outbytesleft = sizeof(output);
  const char *inptr = input;

  size_t result = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
  iconv_close(cd);

  REQUIRE(result == 0);
  REQUIRE(inbytesleft == 0);
}

TEST_CASE("iconv return value - unrepresentable characters")
{
  iconv_t cd = iconv_open("ISO-8859-1//TRANSLIT", "ISO-8859-15");
  REQUIRE(cd != (iconv_t)-1);

  const char input[] = "\xA4";  // € in ISO-8859-15
  size_t inbytesleft = sizeof(input) - 1;
  char output[32];
  char *outptr = output;
  size_t outbytesleft = sizeof(output);
  const char *inptr = input;

  size_t result = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
  iconv_close(cd);

  REQUIRE(result == 1);
}
