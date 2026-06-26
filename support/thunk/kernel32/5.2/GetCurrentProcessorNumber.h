#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall fallback_GetCurrentProcessorNumber();
    DWORD __stdcall zero_GetCurrentProcessorNumber();
  }
} // namespace mingw_thunk
