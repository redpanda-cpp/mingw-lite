#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    UINT __stdcall fallback_GetSystemWow64DirectoryW(_Out_ LPWSTR lpBuffer,
                                                     _In_ UINT uSize);
  }
} // namespace mingw_thunk
