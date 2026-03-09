#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall fallback_GetFinalPathNameByHandleA(_In_ HANDLE hFile,
                                                       _Out_ LPSTR lpszFilePath,
                                                       _In_ DWORD cchFilePath,
                                                       _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
