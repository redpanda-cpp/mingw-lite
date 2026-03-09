#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    int __stdcall fallback_CompareStringOrdinal(_In_ LPCWCH lpString1,
                                                _In_ int cchCount1,
                                                _In_ LPCWCH lpString2,
                                                _In_ int cchCount2,
                                                _In_ BOOL bIgnoreCase);
  }
} // namespace mingw_thunk
