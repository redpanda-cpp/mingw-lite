#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_GetCPInfoExW(_In_ UINT CodePage,
                                      _In_ DWORD dwFlags,
                                      _Out_ LPCPINFOEXW lpCPInfoEx);
  } // namespace f
} // namespace mingw_thunk
