#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_SetProcessAffinityMask(_In_ HANDLE hProcess,
                                    _In_ DWORD_PTR dwProcessAffinityMask);
  }
} // namespace mingw_thunk
