#include "GetLargePageMinimum.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, SIZE_T, WINAPI, GetLargePageMinimum)
  {
    __DISPATCH_THUNK_2(GetLargePageMinimum,
                       const auto pfn = try_get_GetLargePageMinimum(),
                       pfn,
                       &f::fallback_GetLargePageMinimum);

    return dllimport_GetLargePageMinimum();
  }

  namespace f
  {
    SIZE_T __stdcall fallback_GetLargePageMinimum()
    {
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
