#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    VOID __stdcall fallback_DeleteProcThreadAttributeList(
        _Inout_ LPPROC_THREAD_ATTRIBUTE_LIST lpAttributeList);
  }
} // namespace mingw_thunk
