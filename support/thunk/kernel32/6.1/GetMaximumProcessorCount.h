#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall fallback_GetMaximumProcessorCount(_In_ WORD GroupNumber);
  }
} // namespace mingw_thunk
