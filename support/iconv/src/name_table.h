#pragma once

#include "encoding.h"
#include "util/basic_string_view.h"

namespace libiconv
{
  struct name_entry
  {
    string_view name;
    code_page_t page;
  };

  extern const name_entry *const name_table_begin;
  extern const name_entry *const name_table_end;
} // namespace libiconv
