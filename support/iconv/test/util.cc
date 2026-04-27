#include <catch_amalgamated.hpp>
#include <iconv.h>
#include <errno.h>

#include "util.h"

void test_happy_case(const char *to,
                     const char *from,
                     const iconv_case *begin,
                     const iconv_case *end)
{

  for (auto it = begin; it != end; ++it) {
    iconv_t cd = iconv_open(to, from);
    REQUIRE(cd != (iconv_t)-1);

    const char *inptr = it->inbuf();
    size_t inbytesleft = it->inbytes();

    char output[1024];
    char *outptr = output;
    size_t outbytesleft = sizeof(output);

    size_t result = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);

    iconv_close(cd);

    REQUIRE(result != (size_t)-1);
    REQUIRE(inbytesleft == 0);
    REQUIRE(it->expected ==
            std::string_view(output, sizeof(output) - outbytesleft));
  }
}

void test_error_case(const error_test_case *begin,
                     const error_test_case *end)
{
  for (auto it = begin; it != end; ++it)
  {
    INFO(it->name);

    iconv_t cd = iconv_open(it->to, it->from);
    REQUIRE(cd != (iconv_t)-1);

    char output[32];
    char *outptr = output;
    size_t outbytesleft = it->outbuf_size;
    const char *inptr = it->input.data();
    size_t inbytesleft = it->input.size();

    errno = 0;
    size_t result = iconv(cd, &inptr, &inbytesleft, &outptr, &outbytesleft);
    iconv_close(cd);

    REQUIRE(result == (size_t)-1);
    REQUIRE(errno == it->expected_errno);
  }
}
