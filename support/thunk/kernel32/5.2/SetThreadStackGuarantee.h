#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_SetThreadStackGuarantee(_Inout_ PULONG StackSizeInBytes);
  }
} // namespace mingw_thunk
