#pragma once

#include <windows.h>

// after <windows.h>
#include <tlhelp32.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_Module32First(_In_ HANDLE hSnapshot,
                                       _Inout_ LPMODULEENTRY32 lpme);
  } // namespace f
} // namespace mingw_thunk
