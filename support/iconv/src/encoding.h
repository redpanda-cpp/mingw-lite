#pragma once

#include <stdint.h>

namespace libiconv
{
  using code_page_t = unsigned int;

  enum class error_mode
  {
    strict,
    replace,
    ignore
  };

  struct resolve_result
  {
    code_page_t page;
    error_mode mode;
  };

  resolve_result resolve_encoding(const char *code);
} // namespace libiconv

struct iconv_descriptor
{
  libiconv::code_page_t from_page;
  libiconv::code_page_t to_page;
  libiconv::error_mode mode;
};
