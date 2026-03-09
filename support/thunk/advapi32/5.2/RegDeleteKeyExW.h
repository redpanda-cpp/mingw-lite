#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    LSTATUS __stdcall fallback_RegDeleteKeyExW(_In_ HKEY hKey,
                                               _In_ LPCWSTR lpSubKey,
                                               _In_ REGSAM samDesired,
                                               _Reserved_ DWORD Reserved);
  }
} // namespace mingw_thunk
