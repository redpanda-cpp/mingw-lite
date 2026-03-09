#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_SetWaitableTimerEx(_In_ HANDLE _hTimer,
                                _In_ const LARGE_INTEGER *_lpDueTime,
                                _In_ LONG _iPeriod,
                                _In_opt_ PTIMERAPCROUTINE _pfnCompletionRoutine,
                                _In_opt_ LPVOID _lpArgToCompletionRoutine,
                                _In_opt_ PREASON_CONTEXT _pWakeContext,
                                _In_ ULONG _uTolerableDelay);
  }
} // namespace mingw_thunk
