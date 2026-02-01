#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    BOOL win9x_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                                    _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                    _Out_ LPVOID lpFileInformation);

    BOOL win95_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                                    _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                    _Out_ LPVOID lpFileInformation);
  } // namespace impl
} // namespace mingw_thunk
