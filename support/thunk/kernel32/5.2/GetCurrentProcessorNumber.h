#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall fallback_GetCurrentProcessorNumber();
  }
} // namespace mingw_thunk
