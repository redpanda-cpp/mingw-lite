#include <catch_amalgamated.hpp>

#include <iconv.h>
#include <errno.h>

TEST_CASE("iconv bad file descriptor")
{
  const char *input = "A";
  size_t inbytesleft = 1;
  char output[32];
  char *outptr = output;
  size_t outbytesleft = sizeof(output);
  const char *inptr = input;

  errno = 0;
  size_t result = iconv((iconv_t)-1, &inptr, &inbytesleft, &outptr, &outbytesleft);

  REQUIRE(result == (size_t)-1);
  REQUIRE(errno == EBADF);
}
