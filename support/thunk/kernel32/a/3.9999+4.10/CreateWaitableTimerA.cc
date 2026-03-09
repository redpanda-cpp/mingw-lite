#include "CreateWaitableTimerA.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

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
    __DISPATCH_THUNK_2(CreateWaitableTimerA,
                       const auto pfn = try_get_CreateWaitableTimerA(),
                       pfn,
                       &f::win9x_CreateWaitableTimerA);

    return dllimport_CreateWaitableTimerA(
        lpTimerAttributes, bManualReset, lpTimerName);
  }

  namespace f
  {
    HANDLE __stdcall
    win9x_CreateWaitableTimerA(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCSTR lpTimerName)
    {
      SetLastError(ERROR_NOT_SUPPORTED);
      return NULL;
    }
  } // namespace f
} // namespace mingw_thunk
