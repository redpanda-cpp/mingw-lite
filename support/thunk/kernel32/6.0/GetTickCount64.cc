#include "GetTickCount64.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, ULONGLONG, WINAPI, GetTickCount64)
  {
    __DISPATCH_THUNK_2(GetTickCount64,
                       const auto pfn = try_get_GetTickCount64(),
                       pfn,
                       &f::fallback_GetTickCount64);

    return dllimport_GetTickCount64();
  }

  namespace f
  {
    ULONGLONG __stdcall fallback_GetTickCount64()
    {
      return GetTickCount();
    }
  } // namespace f
} // namespace mingw_thunk
