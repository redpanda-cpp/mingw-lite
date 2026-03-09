#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_GetVolumePathNamesForVolumeNameW(_In_ LPCWSTR lpszVolumeName,
                                              _Out_ LPWCH lpszVolumePathNames,
                                              _In_ DWORD cchBufferLength,
                                              _Out_ PDWORD lpcchReturnLength);
  }
} // namespace mingw_thunk
