#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_FlsSetValue(_In_ DWORD dwFlsIndex,
                                        _In_opt_ PVOID lpFlsData);
  }
} // namespace mingw_thunk
