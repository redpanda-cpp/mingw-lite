#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_NeedCurrentDirectoryForExePathW(_In_ LPCWSTR ExeName);
  }
} // namespace mingw_thunk
