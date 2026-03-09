#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    SIZE_T __stdcall fallback_GetLargePageMinimum();
  }
} // namespace mingw_thunk
