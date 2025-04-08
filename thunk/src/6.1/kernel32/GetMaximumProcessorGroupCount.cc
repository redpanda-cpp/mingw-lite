#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Minimum supported client	Windows 7 [desktop apps only]
  // Minimum supported server	Windows Server 2008 R2 [desktop apps only]
  __DEFINE_THUNK(kernel32, 0, WORD, WINAPI, GetMaximumProcessorGroupCount, VOID)
  {
    if (auto pGetMaximumProcessorGroupCount =
            try_get_GetMaximumProcessorGroupCount()) {
      return pGetMaximumProcessorGroupCount();
    }

    // 我们统一假定只有一组，事实也如此。
    return 1;
  }
} // namespace mingw_thunk
