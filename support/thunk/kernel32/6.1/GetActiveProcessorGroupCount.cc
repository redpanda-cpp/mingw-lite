#include "GetActiveProcessorGroupCount.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 7 [desktop apps only]
  // Minimum supported server	Windows Server 2008 R2 [desktop apps only]
  __DEFINE_THUNK(kernel32, 0, WORD, WINAPI, GetActiveProcessorGroupCount, VOID)
  {
    __DISPATCH_THUNK_2(GetActiveProcessorGroupCount,
                       const auto pfn = try_get_GetActiveProcessorGroupCount(),
                       pfn,
                       &f::fallback_GetActiveProcessorGroupCount);

    return dllimport_GetActiveProcessorGroupCount();
  }

  namespace f
  {
    WORD __stdcall fallback_GetActiveProcessorGroupCount(VOID)
    {
      // 我们统一假定只有一组，事实也如此。
      return 1;
    }
  } // namespace f
} // namespace mingw_thunk
