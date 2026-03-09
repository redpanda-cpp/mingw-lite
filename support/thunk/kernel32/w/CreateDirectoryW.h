#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    win9x_CreateDirectoryW(_In_ LPCWSTR lpPathName,
                           _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes);
  }
} // namespace mingw_thunk