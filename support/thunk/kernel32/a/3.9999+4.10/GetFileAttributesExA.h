#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    win9x_GetFileAttributesExA(_In_ LPCSTR lpFileName,
                               _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                               _Out_ LPVOID lpFileInformation);
  }
} // namespace mingw_thunk
