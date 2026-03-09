#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    LCID __stdcall fallback_LocaleNameToLCID(_In_ LPCWSTR lpName,
                                             _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
