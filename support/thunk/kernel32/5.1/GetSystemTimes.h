#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_GetSystemTimes(_Out_opt_ PFILETIME lpIdleTime,
                                           _Out_opt_ PFILETIME lpKernelTime,
                                           _Out_opt_ PFILETIME lpUserTime);
  }
} // namespace mingw_thunk
