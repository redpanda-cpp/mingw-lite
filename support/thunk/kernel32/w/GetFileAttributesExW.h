#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    win98_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                                     _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                     _Out_ LPVOID lpFileInformation);

    BOOL __stdcall
    win95_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                               _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                               _Out_ LPVOID lpFileInformation);
  } // namespace f
} // namespace mingw_thunk
