#include "GetMaximumProcessorCount.h"

#include <thunk/_common.h>
#include <thunk/yy/api-ms-win-core-processtopology-obsolete.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 7 [desktop apps only]
  // Minimum supported server	Windows Server 2008 R2 [desktop apps only]
  __DEFINE_THUNK(kernel32,
                 4,
                 DWORD,
                 WINAPI,
                 GetMaximumProcessorCount,
                 _In_ WORD GroupNumber)
  {
    __DISPATCH_THUNK_2(GetMaximumProcessorCount,
                       const auto pfn = try_get_GetMaximumProcessorCount(),
                       pfn,
                       &f::fallback_GetMaximumProcessorCount);

    return dllimport_GetMaximumProcessorCount(GroupNumber);
  }

  namespace f
  {
    DWORD __stdcall fallback_GetMaximumProcessorCount(_In_ WORD GroupNumber)
    {
      return Downlevel::GetProcessorCount(GroupNumber);
    }
  } // namespace f
} // namespace mingw_thunk
