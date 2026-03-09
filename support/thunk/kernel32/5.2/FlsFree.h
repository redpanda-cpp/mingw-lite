#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_FlsFree(_In_ DWORD dwFlsIndex);
  }
} // namespace mingw_thunk
