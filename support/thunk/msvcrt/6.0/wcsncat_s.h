#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace impl
  {
    errno_t fallback_wcsncat_s(wchar_t *strDest,
                               size_t numberOfElements,
                               const wchar_t *strSource,
                               size_t count);
  } // namespace impl
} // namespace mingw_thunk
