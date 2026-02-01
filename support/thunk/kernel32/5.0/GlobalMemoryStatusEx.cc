#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 4,
                 BOOL,
                 WINAPI,
                 GlobalMemoryStatusEx,
                 _Inout_ LPMEMORYSTATUSEX lpBuffer)
  {
    if (const auto pfn = try_get_GlobalMemoryStatusEx())
      return pfn(lpBuffer);

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
} // namespace mingw_thunk
