#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    DWORD
    win9x_SearchPathW(_In_opt_ LPCWSTR lpPath,
                      _In_ LPCWSTR lpFileName,
                      _In_opt_ LPCWSTR lpExtension,
                      _In_ DWORD nBufferLength,
                      _Out_ LPWSTR lpBuffer,
                      _Out_opt_ LPWSTR *lpFilePart);
  } // namespace impl
} // namespace mingw_thunk
