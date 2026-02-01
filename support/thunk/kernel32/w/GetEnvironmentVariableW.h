#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    DWORD
    win9x_GetEnvironmentVariableW(_In_opt_ LPCWSTR lpName,
                                  _Out_opt_ LPWSTR lpBuffer,
                                  _In_ DWORD nSize);
  } // namespace impl
} // namespace mingw_thunk
