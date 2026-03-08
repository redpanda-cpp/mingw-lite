#include <thunk/_common.h>
#include <thunk/findvolumedata.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, BOOL, WINAPI, FindVolumeClose, _In_ HANDLE hFindVolume)
  {
    if (const auto pfn = try_get_FindVolumeClose())
      return pfn(hFindVolume);

    HeapFree(GetProcessHeap(), 0, hFindVolume);
    return TRUE;
  }
} // namespace mingw_thunk
