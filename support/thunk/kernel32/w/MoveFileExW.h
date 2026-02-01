#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    BOOL win9x_MoveFileExW(_In_ LPCWSTR lpExistingFileName,
                           _In_opt_ LPCWSTR lpNewFileName,
                           _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
