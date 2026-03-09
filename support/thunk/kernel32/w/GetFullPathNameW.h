#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall win9x_GetFullPathNameW(_In_ LPCWSTR lpFileName,
                                           _In_ DWORD nBufferLength,
                                           _Out_ LPWSTR lpBuffer,
                                           _Out_opt_ LPWSTR *lpFilePart);
  } // namespace f
} // namespace mingw_thunk
