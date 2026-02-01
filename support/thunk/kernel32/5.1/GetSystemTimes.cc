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
    if (const auto pfn = try_get_GetSystemTimes())
      return pfn(lpIdleTime, lpKernelTime, lpUserTime);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
