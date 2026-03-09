#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win9x_CreateFileW(_In_ LPCWSTR lpFileName,
                      _In_ DWORD dwDesiredAccess,
                      _In_ DWORD dwShareMode,
                      _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes,
                      _In_ DWORD dwCreationDisposition,
                      _In_ DWORD dwFlagsAndAttributes,
                      _In_opt_ HANDLE hTemplateFile);
  } // namespace f
} // namespace mingw_thunk
