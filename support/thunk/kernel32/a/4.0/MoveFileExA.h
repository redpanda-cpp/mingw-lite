#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_MoveFileExA(_In_ LPCSTR lpExistingFileName,
                                     _In_opt_ LPCSTR lpNewFileName,
                                     _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
