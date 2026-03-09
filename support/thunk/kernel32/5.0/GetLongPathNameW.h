#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD WINAPI win98_GetLongPathNameW(_In_ LPCWSTR lpszShortPath,
                                        _Out_ LPWSTR lpszLongPath,
                                        _In_ DWORD cchBuffer);

    DWORD WINAPI noop_GetLongPathNameW(_In_ LPCWSTR lpszShortPath,
                                       _Out_ LPWSTR lpszLongPath,
                                       _In_ DWORD cchBuffer);
  } // namespace f
} // namespace mingw_thunk
