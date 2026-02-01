#include "GetSystemTimePreciseAsFileTime.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  __DEFINE_THUNK(kernel32,
                 4,
                 VOID,
                 WINAPI,
                 GetSystemTimePreciseAsFileTime,
                 _Out_ LPFILETIME lpSystemTimeAsFileTime)
  {
    if (auto const pGetSystemTimePreciseAsFileTime =
            try_get_GetSystemTimePreciseAsFileTime()) {
      return pGetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
    }

    return impl::fallback_GetSystemTimePreciseAsFileTime(
        lpSystemTimeAsFileTime);
  }

  namespace impl
  {
    VOID fallback_GetSystemTimePreciseAsFileTime(
        _Out_ LPFILETIME lpSystemTimeAsFileTime)
    {
      return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    }
  } // namespace impl
} // namespace mingw_thunk
