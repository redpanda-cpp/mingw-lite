#include "SetWaitableTimer.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

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
    __DISPATCH_THUNK_2(SetWaitableTimer,
                       const auto pfn = try_get_SetWaitableTimer(),
                       pfn,
                       &f::fallback_SetWaitableTimer);

    return dllimport_SetWaitableTimer(hTimer,
                                      lpDueTime,
                                      lPeriod,
                                      pfnCompletionRoutine,
                                      lpArgToCompletionRoutine,
                                      fResume);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_SetWaitableTimer(_In_ HANDLE hTimer,
                              _In_ const LARGE_INTEGER *lpDueTime,
                              _In_ LONG lPeriod,
                              _In_opt_ PTIMERAPCROUTINE pfnCompletionRoutine,
                              _In_opt_ LPVOID lpArgToCompletionRoutine,
                              _In_ BOOL fResume)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
