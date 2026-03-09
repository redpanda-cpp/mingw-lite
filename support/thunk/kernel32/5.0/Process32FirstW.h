#pragma once

#include <windows.h>

// after <windows.h>
#include <tlhelp32.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Process32FirstW(_In_ HANDLE hSnapshot,
                                         _Inout_ LPPROCESSENTRY32W lppe);
  } // namespace f
} // namespace mingw_thunk
