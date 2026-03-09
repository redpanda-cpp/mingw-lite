#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    fallback_CreateToolhelp32Snapshot(_In_ DWORD dwFlags,
                                      _In_ DWORD th32ProcessID);
  } // namespace f
} // namespace mingw_thunk
