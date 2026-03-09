#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win98_CreateWaitableTimerW(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCWSTR lpTimerName);

    HANDLE __stdcall
    win95_CreateWaitableTimerW(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCWSTR lpTimerName);
  } // namespace f
} // namespace mingw_thunk
