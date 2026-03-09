#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    WORD __stdcall fallback_GetMaximumProcessorGroupCount(VOID);
  }
} // namespace mingw_thunk
