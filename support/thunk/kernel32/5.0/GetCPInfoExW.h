#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    BOOL fallback_GetCPInfoExW(_In_ UINT CodePage,
                               _In_ DWORD dwFlags,
                               _Out_ LPCPINFOEXW lpCPInfoEx);
  }
} // namespace mingw_thunk
