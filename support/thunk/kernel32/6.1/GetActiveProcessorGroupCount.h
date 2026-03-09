#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    WORD __stdcall fallback_GetActiveProcessorGroupCount(VOID);
  }
} // namespace mingw_thunk
