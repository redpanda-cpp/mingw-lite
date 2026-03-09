#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace f
  {
    errno_t fallback__wputenv_s(const wchar_t *varname,
                                const wchar_t *value_string);
  } // namespace f
} // namespace mingw_thunk
