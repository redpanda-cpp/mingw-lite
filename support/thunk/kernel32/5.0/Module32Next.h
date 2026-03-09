#pragma once

#include <windows.h>

// after <windows.h>
#include <tlhelp32.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Module32Next(_In_ HANDLE hSnapshot,
                                      _Out_ LPMODULEENTRY32 lpme);
  } // namespace f
} // namespace mingw_thunk
