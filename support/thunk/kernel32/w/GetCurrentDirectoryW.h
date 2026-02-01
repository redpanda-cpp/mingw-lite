#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    DWORD
    win9x_GetCurrentDirectoryW(_In_ DWORD nBufferLength, _Out_ LPWSTR lpBuffer);
  } // namespace impl
} // namespace mingw_thunk
