#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_GetCPInfoExA(_In_ UINT CodePage,
                                         _In_ DWORD dwFlags,
                                         _Out_ LPCPINFOEXA lpCPInfoEx);
  }
} // namespace mingw_thunk
