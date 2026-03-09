#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall fallback_CreateWaitableTimerExW(
        _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
        _In_opt_ LPCWSTR lpTimerName,
        _In_ DWORD dwFlags,
        _In_ DWORD dwDesiredAccess);
  }
} // namespace mingw_thunk
