#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_IsDebuggerPresent(void);
  }
} // namespace mingw_thunk
