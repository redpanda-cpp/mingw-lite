#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    VOID __stdcall
    winnt_GetCurrentThreadStackLimits(_Out_ PULONG_PTR LowLimit,
                                      _Out_ PULONG_PTR HighLimit);

    VOID __stdcall
    win9x_GetCurrentThreadStackLimits(_Out_ PULONG_PTR LowLimit,
                                      _Out_ PULONG_PTR HighLimit);
  } // namespace f
} // namespace mingw_thunk
