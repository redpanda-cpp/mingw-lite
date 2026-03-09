#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_GetVolumePathNameW(_In_ LPCWSTR lpszFileName,
                                            _Out_ LPWSTR lpszVolumePathName,
                                            _In_ DWORD cchBufferLength);
  } // namespace f
} // namespace mingw_thunk
