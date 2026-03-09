#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    ULONGLONG __stdcall fallback_GetTickCount64();
  }
} // namespace mingw_thunk
