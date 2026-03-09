#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    UINT __stdcall win32_GetSystemWow64DirectoryA(_Out_ LPSTR lpBuffer,
                                                  _In_ UINT uSize);
  }
} // namespace mingw_thunk
