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
    __DISPATCH_THUNK_2(GetSystemTimePreciseAsFileTime,
                       const auto pfn =
                           try_get_GetSystemTimePreciseAsFileTime(),
                       pfn,
                       &f::fallback_GetSystemTimePreciseAsFileTime);

    return dllimport_GetSystemTimePreciseAsFileTime(lpSystemTimeAsFileTime);
  }

  namespace f
  {
    VOID __stdcall fallback_GetSystemTimePreciseAsFileTime(
        _Out_ LPFILETIME lpSystemTimeAsFileTime)
    {
      return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
    }
  } // namespace f
} // namespace mingw_thunk
