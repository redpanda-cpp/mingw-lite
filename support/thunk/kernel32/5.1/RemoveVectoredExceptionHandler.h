#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    ULONG __stdcall fallback_RemoveVectoredExceptionHandler(PVOID Handle);
  }
} // namespace mingw_thunk
