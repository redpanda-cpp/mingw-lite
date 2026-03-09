#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win9x_CreateEventW(_In_opt_ LPSECURITY_ATTRIBUTES lpEventAttributes,
                       _In_ BOOL bManualReset,
                       _In_ BOOL bInitialState,
                       _In_opt_ LPCWSTR lpName);
  }
} // namespace mingw_thunk