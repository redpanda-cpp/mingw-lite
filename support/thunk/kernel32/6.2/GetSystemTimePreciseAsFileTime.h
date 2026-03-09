#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    VOID __stdcall fallback_GetSystemTimePreciseAsFileTime(
        _Out_ LPFILETIME lpSystemTimeAsFileTime);
  }
} // namespace mingw_thunk
