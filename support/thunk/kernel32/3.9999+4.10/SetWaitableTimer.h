#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_SetWaitableTimer(_In_ HANDLE hTimer,
                              _In_ const LARGE_INTEGER *lpDueTime,
                              _In_ LONG lPeriod,
                              _In_opt_ PTIMERAPCROUTINE pfnCompletionRoutine,
                              _In_opt_ LPVOID lpArgToCompletionRoutine,
                              _In_ BOOL fResume);
  }
} // namespace mingw_thunk
