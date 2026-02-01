#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    DWORD
    win9x_GetModuleFileNameW(_In_opt_ HMODULE hModule,
                             _Out_ LPWSTR lpFilename,
                             _In_ DWORD nSize);
  } // namespace impl
} // namespace mingw_thunk
