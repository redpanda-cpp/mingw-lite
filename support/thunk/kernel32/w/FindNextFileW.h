#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_FindNextFileW(_In_ HANDLE hFindFile,
                                       _Out_ LPWIN32_FIND_DATAW lpFindFileData);
  }
} // namespace mingw_thunk
