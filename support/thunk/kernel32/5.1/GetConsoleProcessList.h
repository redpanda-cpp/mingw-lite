#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall
    fallback_GetConsoleProcessList(_Out_ LPDWORD lpdwProcessList,
                                   _In_ DWORD dwProcessCount);
  }
} // namespace mingw_thunk
