#pragma once

#include <stddef.h>
#include <string_view>

struct iconv_case
{
  std::string_view input;
  std::string_view expected;

  const char *inbuf() const
  {
    return input.data();
  }

  size_t inbytes() const
  {
    return input.size();
  }
};

void test_happy_case(const char *to,
                     const char *from,
                     const iconv_case *begin,
                     const iconv_case *end);

struct error_test_case
{
  const char *name;
  const char *to;
  const char *from;
  std::string_view input;
  size_t outbuf_size;
  int expected_errno;
};

void test_error_case(const error_test_case *begin,
                     const error_test_case *end);
