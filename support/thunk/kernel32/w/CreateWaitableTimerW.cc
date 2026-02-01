#include "CreateWaitableTimerW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <nostl/string.h>

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
    if (internal::is_nt()) {
#if THUNK_LEVEL >= NTDDI_WIN98
      return __ms_CreateWaitableTimerW(
          lpTimerAttributes, bManualReset, lpTimerName);
#else
      return get_CreateWaitableTimerW()(
          lpTimerAttributes, bManualReset, lpTimerName);
#endif
    }

    return impl::win9x_CreateWaitableTimerW(
        lpTimerAttributes, bManualReset, lpTimerName);
  }

  namespace impl
  {
    HANDLE
    win9x_CreateWaitableTimerW(_In_opt_ LPSECURITY_ATTRIBUTES lpTimerAttributes,
                               _In_ BOOL bManualReset,
                               _In_opt_ LPCWSTR lpTimerName)
    {
#if THUNK_LEVEL >= NTDDI_WIN98
      stl::string a_name;
      if (lpTimerName)
        a_name = internal::w2a(lpTimerName);
      return __ms_CreateWaitableTimerA(lpTimerAttributes,
                                       bManualReset,
                                       lpTimerName ? a_name.c_str() : nullptr);
#else
      if (internal::os_geq(VER_PLATFORM_WIN32_WINDOWS, 4, 10)) {
        stl::string a_name;
        if (lpTimerName)
          a_name = internal::w2a(lpTimerName);
        return kernel32_CreateWaitableTimerA()(lpTimerAttributes,
                                               bManualReset,
                                               lpTimerName ? a_name.c_str()
                                                           : nullptr);
      } else {
        SetLastError(ERROR_NOT_SUPPORTED);
        return nullptr;
      }
#endif
    }
  } // namespace impl
} // namespace mingw_thunk
