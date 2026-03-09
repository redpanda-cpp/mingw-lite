#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win9x_CreateSemaphoreW(_In_opt_ LPSECURITY_ATTRIBUTES lpSemaphoreAttributes,
                           _In_ LONG lInitialCount,
                           _In_ LONG lMaximumCount,
                           _In_opt_ LPCWSTR lpName);
  }
} // namespace mingw_thunk
