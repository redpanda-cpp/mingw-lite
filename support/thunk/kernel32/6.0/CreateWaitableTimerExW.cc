#include "CreateWaitableTimerExW.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  // YY-Thunks 1.1.6
  // Windows Vista [desktop apps | UWP apps]
  // Windows Server 2008 [desktop apps | UWP apps]
  __DEFINE_THUNK(kernel32,
                 16,
                 HANDLE,
                 WINAPI,
                 CreateWaitableTimerExW,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                 _In_opt_ LPCWSTR lpTimerName,
                 _In_ DWORD dwFlags,
                 _In_ DWORD dwDesiredAccess)
  {
    __DISPATCH_THUNK_2(CreateWaitableTimerExW,
                       const auto pfn = try_get_CreateWaitableTimerExW(),
                       pfn,
                       &f::fallback_CreateWaitableTimerExW);

    return dllimport_CreateWaitableTimerExW(
        lpTimerAttributes, lpTimerName, dwFlags, dwDesiredAccess);
  }

  namespace f
  {
    HANDLE __stdcall fallback_CreateWaitableTimerExW(
        _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
        _In_opt_ LPCWSTR lpTimerName,
        _In_ DWORD dwFlags,
        _In_ DWORD dwDesiredAccess)
    {
      return CreateWaitableTimerW(lpTimerAttributes,
                                  dwFlags & CREATE_WAITABLE_TIMER_MANUAL_RESET,
                                  lpTimerName);
    }
  } // namespace f
} // namespace mingw_thunk
