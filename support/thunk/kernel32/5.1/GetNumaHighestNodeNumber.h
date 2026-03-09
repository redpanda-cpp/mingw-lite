#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_GetNumaHighestNodeNumber(_Out_ PULONG HighestNodeNumber);
  }
} // namespace mingw_thunk
