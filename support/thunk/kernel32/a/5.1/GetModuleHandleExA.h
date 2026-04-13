#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_GetModuleHandleExA(_In_ DWORD dwFlags,
                                               _In_opt_ LPCSTR lpModuleName,
                                               _Out_ HMODULE *phModule);
  }
} // namespace mingw_thunk
