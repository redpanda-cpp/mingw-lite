#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall win9x_GetTempPathW(_In_ DWORD nBufferLength,
                                       _Out_ LPWSTR lpBuffer);
  } // namespace f
} // namespace mingw_thunk
