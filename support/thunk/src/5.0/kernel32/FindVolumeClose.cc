#include <thunk/_common.h>
#include <thunk/findvolumedata.h>
#include <thunk/libc/stdlib.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, BOOL, WINAPI, FindVolumeClose, _In_ HANDLE hFindVolume)
  {
    if (const auto pfn = try_get_FindVolumeClose())
      return pfn(hFindVolume);

    internal::free(hFindVolume);
    return TRUE;
  }
} // namespace mingw_thunk
