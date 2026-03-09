#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall win9x_GetEnvironmentVariableW(_In_opt_ LPCWSTR lpName,
                                                  _Out_opt_ LPWSTR lpBuffer,
                                                  _In_ DWORD nSize);
  } // namespace f
} // namespace mingw_thunk
