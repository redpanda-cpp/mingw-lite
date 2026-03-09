#include "GetSystemTimes.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetSystemTimes,
                 _Out_opt_ PFILETIME lpIdleTime,
                 _Out_opt_ PFILETIME lpKernelTime,
                 _Out_opt_ PFILETIME lpUserTime)
  {
    __DISPATCH_THUNK_2(GetSystemTimes,
                       const auto pfn = try_get_GetSystemTimes(),
                       pfn,
                       &f::fallback_GetSystemTimes);

    return dllimport_GetSystemTimes(lpIdleTime, lpKernelTime, lpUserTime);
  }

  namespace f
  {
    BOOL __stdcall fallback_GetSystemTimes(_Out_opt_ PFILETIME lpIdleTime,
                                           _Out_opt_ PFILETIME lpKernelTime,
                                           _Out_opt_ PFILETIME lpUserTime)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
