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
                 GetActiveProcessorCount,
                 _In_ WORD GroupNumber)
  {
    if (auto pGetActiveProcessorCount = try_get_GetActiveProcessorCount()) {
      return pGetActiveProcessorCount(GroupNumber);
    }

    return Downlevel::GetProcessorCount(GroupNumber);
  }
} // namespace mingw_thunk
