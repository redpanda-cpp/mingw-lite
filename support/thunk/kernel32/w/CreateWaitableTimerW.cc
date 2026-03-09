#include "CreateWaitableTimerW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 98 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 HANDLE,
                 WINAPI,
                 CreateWaitableTimerW,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                 _In_ BOOL bManualReset,
                 _In_opt_ LPCWSTR lpTimerName)
  {
#if THUNK_LEVEL >= NTDDI_WIN98
    __DISPATCH_THUNK_2(CreateWaitableTimerW,
                       i::is_nt(),
                       &__ms_CreateWaitableTimerW,
                       &f::win98_CreateWaitableTimerW);
#else
    __DISPATCH_THUNK_3(CreateWaitableTimerW,
                       i::is_nt(),
                       get_CreateWaitableTimerW(),
                       i::os_version() >= g::win32_win98,
                       &f::win98_CreateWaitableTimerW,
                       &f::win95_CreateWaitableTimerW);
#endif

    return dllimport_CreateWaitableTimerW(
        lpTimerAttributes, bManualReset, lpTimerName);
  }

  namespace f
  {
    HANDLE __stdcall
    win98_CreateWaitableTimerW(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCWSTR lpTimerName)
    {
      d::a_str a_name;
      if (lpTimerName && !a_name.from_w(lpTimerName)) {
        SetLastError(ERROR_OUTOFMEMORY);
        return nullptr;
      }
#if THUNK_LEVEL >= NTDDI_WIN98
      const auto CreateWaitableTimerA = __ms_CreateWaitableTimerA;
#else
      const auto CreateWaitableTimerA = kernel32_CreateWaitableTimerA();
#endif
      return CreateWaitableTimerA(lpTimerAttributes,
                                  bManualReset,
                                  lpTimerName ? a_name.c_str() : nullptr);
    }

    HANDLE __stdcall
    win95_CreateWaitableTimerW(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCWSTR lpTimerName)
    {
      SetLastError(ERROR_NOT_SUPPORTED);
      return nullptr;
    }
  } // namespace f
} // namespace mingw_thunk
