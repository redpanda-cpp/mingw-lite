#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI
    fallback_GlobalMemoryStatusEx(_Inout_ LPMEMORYSTATUSEX lpBuffer);
  } // namespace f
} // namespace mingw_thunk
