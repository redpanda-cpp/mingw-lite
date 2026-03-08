#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 HANDLE,
                 WINAPI,
                 CreateWaitableTimerA,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                 _In_ BOOL bManualReset,
                 _In_opt_ LPCSTR lpTimerName)
  {
    d::w_str w_timer_name;
    if (lpTimerName && !w_timer_name.from_u(lpTimerName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return nullptr;
    }

    return CreateWaitableTimerW(lpTimerAttributes,
                                bManualReset,
                                lpTimerName ? w_timer_name.c_str() : nullptr);
  }
} // namespace mingw_thunk
