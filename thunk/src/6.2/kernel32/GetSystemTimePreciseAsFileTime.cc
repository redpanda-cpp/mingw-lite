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

    return GetSystemTimeAsFileTime(lpSystemTimeAsFileTime);
  }
} // namespace mingw_thunk
