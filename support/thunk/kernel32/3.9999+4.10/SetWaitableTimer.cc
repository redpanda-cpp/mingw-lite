#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 24,
                 BOOL,
                 WINAPI,
                 SetWaitableTimer,
                 _In_ HANDLE hTimer,
                 _In_ const LARGE_INTEGER *lpDueTime,
                 _In_ LONG lPeriod,
                 _In_opt_ PTIMERAPCROUTINE pfnCompletionRoutine,
                 _In_opt_ LPVOID lpArgToCompletionRoutine,
                 _In_ BOOL fResume)
  {
    if (const auto pfn = try_get_SetWaitableTimer())
      return pfn(hTimer,
                 lpDueTime,
                 lPeriod,
                 pfnCompletionRoutine,
                 lpArgToCompletionRoutine,
                 fResume);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
