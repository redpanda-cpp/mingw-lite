#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall fallback_GetModuleFileNameExW(_In_ HANDLE hProcess,
                                                  _In_opt_ HMODULE hModule,
                                                  _Out_ LPWSTR lpFilename,
                                                  _In_ DWORD nSize);
  }
} // namespace mingw_thunk
