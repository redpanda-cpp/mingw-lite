#include "GlobalMemoryStatusEx.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI
    fallback_GlobalMemoryStatusEx(_Inout_ LPMEMORYSTATUSEX lpBuffer);
  }

  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 GlobalMemoryStatusEx,
                 _Inout_ LPMEMORYSTATUSEX lpBuffer)
  {
    __DISPATCH_THUNK_2(GlobalMemoryStatusEx,
                       const auto pfn = try_get_GlobalMemoryStatusEx(),
                       pfn,
                       &f::fallback_GlobalMemoryStatusEx);

    return dllimport_GlobalMemoryStatusEx(lpBuffer);
  }

  namespace f
  {
    BOOL WINAPI fallback_GlobalMemoryStatusEx(_Inout_ LPMEMORYSTATUSEX lpBuffer)
    {
      MEMORYSTATUS ms = {sizeof(MEMORYSTATUS)};
      GlobalMemoryStatus(&ms);

      lpBuffer->dwMemoryLoad = ms.dwMemoryLoad;
      lpBuffer->ullTotalPhys = ms.dwTotalPhys;
      lpBuffer->ullAvailPhys = ms.dwAvailPhys;
      lpBuffer->ullTotalPageFile = ms.dwTotalPageFile;
      lpBuffer->ullAvailPageFile = ms.dwAvailPageFile;
      lpBuffer->ullTotalVirtual = ms.dwTotalVirtual;
      lpBuffer->ullAvailVirtual = ms.dwAvailVirtual;
      lpBuffer->ullAvailExtendedVirtual = 0;
      return TRUE;
    }
  } // namespace f
} // namespace mingw_thunk
