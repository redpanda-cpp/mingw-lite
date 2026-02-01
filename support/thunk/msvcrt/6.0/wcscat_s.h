#pragma once

#include <wchar.h>

namespace mingw_thunk
{
  namespace impl
  {
    errno_t
    fallback_wcscat_s(wchar_t *strDestination, size_t numberOfElements, const wchar_t *strSource);
  } // namespace impl
} // namespace mingw_thunk
