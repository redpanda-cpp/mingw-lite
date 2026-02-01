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
    if (auto pCreateWaitableTimerExW = try_get_CreateWaitableTimerExW()) {
      return pCreateWaitableTimerExW(
          lpTimerAttributes, lpTimerName, dwFlags, dwDesiredAccess);
    }

    return CreateWaitableTimerW(lpTimerAttributes,
                                dwFlags & CREATE_WAITABLE_TIMER_MANUAL_RESET,
                                lpTimerName);
  }
} // namespace mingw_thunk
