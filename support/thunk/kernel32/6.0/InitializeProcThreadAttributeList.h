#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_InitializeProcThreadAttributeList(
        _Out_opt_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList,
        _In_ DWORD dwAttributeCount,
        DWORD dwFlags,
        _Inout_ PSIZE_T lpSize);
  }
} // namespace mingw_thunk
