#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOLEAN __stdcall
    fallback_CreateSymbolicLinkW(_In_ LPCWSTR lpSymlinkFileName,
                                 _In_ LPCWSTR lpTargetFileName,
                                 _In_ DWORD dwFlags);
  }
} // namespace mingw_thunk
