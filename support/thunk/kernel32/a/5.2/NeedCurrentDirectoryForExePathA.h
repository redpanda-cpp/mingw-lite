#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_NeedCurrentDirectoryForExePathA(_In_ LPCSTR ExeName);
  }
} // namespace mingw_thunk
