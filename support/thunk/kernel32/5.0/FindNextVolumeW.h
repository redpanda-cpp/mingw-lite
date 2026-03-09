#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_FindNextVolumeW(_In_ HANDLE hFindVolume,
                                            _Out_ LPWSTR lpszVolumeName,
                                            _In_ DWORD cchBufferLength);
  } // namespace f
} // namespace mingw_thunk
