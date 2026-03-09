#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HANDLE __stdcall fallback_FindFirstVolumeW(_Out_ LPWSTR lpszVolumeName,
                                               _In_ DWORD cchBufferLength);
  } // namespace f
} // namespace mingw_thunk
