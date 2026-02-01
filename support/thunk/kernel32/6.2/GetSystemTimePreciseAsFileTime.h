#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    VOID fallback_GetSystemTimePreciseAsFileTime(
        _Out_ LPFILETIME lpSystemTimeAsFileTime);
  } // namespace impl
} // namespace mingw_thunk
