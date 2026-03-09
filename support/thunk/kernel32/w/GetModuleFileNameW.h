#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall win9x_GetModuleFileNameW(_In_opt_ HMODULE hModule,
                                             _Out_ LPWSTR lpFilename,
                                             _In_ DWORD nSize);
  } // namespace f
} // namespace mingw_thunk
