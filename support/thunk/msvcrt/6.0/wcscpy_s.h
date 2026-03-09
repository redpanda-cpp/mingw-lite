#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace f
  {
    errno_t
    fallback_wcscpy_s(wchar_t *dest, rsize_t dest_size, const wchar_t *src);
  } // namespace f
} // namespace mingw_thunk
