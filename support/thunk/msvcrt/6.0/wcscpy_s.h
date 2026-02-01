#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace impl
  {
    errno_t
    fallback_wcscpy_s(wchar_t *dest, rsize_t dest_size, const wchar_t *src);
  } // namespace impl
} // namespace mingw_thunk
