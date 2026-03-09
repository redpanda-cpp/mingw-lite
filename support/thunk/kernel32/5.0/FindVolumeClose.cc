#include "FindVolumeClose.h"

#include <thunk/_common.h>
#include <thunk/findvolumedata.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, BOOL, WINAPI, FindVolumeClose, _In_ HANDLE hFindVolume)
  {
    __DISPATCH_THUNK_2(FindVolumeClose,
                       const auto pfn = try_get_FindVolumeClose(),
                       pfn,
                       &f::fallback_FindVolumeClose);

    return dllimport_FindVolumeClose(hFindVolume);
  }

  namespace f
  {
    BOOL __stdcall fallback_FindVolumeClose(_In_ HANDLE hFindVolume)
    {
      HeapFree(GetProcessHeap(), 0, hFindVolume);
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
