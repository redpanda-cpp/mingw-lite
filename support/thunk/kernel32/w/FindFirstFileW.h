#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall
    win9x_FindFirstFileW(_In_ LPCWSTR lpFileName,
                         _Out_ LPWIN32_FIND_DATAW lpFindFileData);
  }
} // namespace mingw_thunk
