#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    PVOID __stdcall fallback_FlsGetValue(_In_ DWORD dwFlsIndex);
  }
} // namespace mingw_thunk
