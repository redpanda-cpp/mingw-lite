#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win9x_CreateWaitableTimerA(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCSTR lpTimerName);
  }
} // namespace mingw_thunk
