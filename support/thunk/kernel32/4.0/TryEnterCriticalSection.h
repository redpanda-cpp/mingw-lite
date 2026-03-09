#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    win9x_TryEnterCriticalSection(_Inout_ LPCRITICAL_SECTION lpCriticalSection);
  } // namespace f
} // namespace mingw_thunk
