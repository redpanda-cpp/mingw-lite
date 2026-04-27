#include <catch_amalgamated.hpp>
#include <iconv.h>

TEST_CASE("iconv_open invalid name")
{
  iconv_t cd;

  errno = 0;
  cd = iconv_open("invalid", "UTF-8");
  REQUIRE(cd == (iconv_t)-1);
  REQUIRE(errno == EINVAL);

  errno = 0;
  cd = iconv_open("UTF-8", "utf8");
  REQUIRE(cd == (iconv_t)-1);
  REQUIRE(errno == EINVAL);

  cd = iconv_open(nullptr, nullptr);
  REQUIRE(cd == (iconv_t)-1);
  REQUIRE(errno == EINVAL);
}
