#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOLEAN __stdcall
    fallback_CreateSymbolicLinkA(_In_ LPCSTR lpSymlinkFileName,
                                 _In_ LPCSTR lpTargetFileName,
                                 _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
