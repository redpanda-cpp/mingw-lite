#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    HANDLE
    win9x_CreateWaitableTimerW(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCWSTR lpTimerName);
  }
} // namespace mingw_thunk
