#include <thunk/_common.h>

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
    if (const auto pfn = try_get_CreateWaitableTimerA())
      return pfn(lpTimerAttributes, bManualReset, lpTimerName);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return NULL;
  }
} // namespace mingw_thunk
