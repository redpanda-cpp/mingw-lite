#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace f
  {
    errno_t fallback_wcsncpy_s(wchar_t *strDest,
                               size_t numberOfElements,
                               const wchar_t *strSource,
                               size_t count);
  } // namespace f
} // namespace mingw_thunk
