#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace f
  {
    errno_t fallback_wcscat_s(wchar_t *strDestination,
                              size_t numberOfElements,
                              const wchar_t *strSource);
  } // namespace f
} // namespace mingw_thunk
