#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32, 0, SIZE_T, WINAPI, GetLargePageMinimum)
  {
    if (const auto pfn = try_get_GetLargePageMinimum())
      return pfn();

    return 0;
  }
} // namespace mingw_thunk
