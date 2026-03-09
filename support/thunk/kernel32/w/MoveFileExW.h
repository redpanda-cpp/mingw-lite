#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_MoveFileExW(_In_ LPCWSTR lpExistingFileName,
                                     _In_opt_ LPCWSTR lpNewFileName,
                                     _In_ DWORD dwFlags);
  } // namespace f
} // namespace mingw_thunk
