#pragma once

#include <windows.h>

// after <windows.h>
#include <tlhelp32.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Process32NextW(_In_ HANDLE hSnapshot,
                                        _Out_ LPPROCESSENTRY32W lppe);
  } // namespace f
} // namespace mingw_thunk
