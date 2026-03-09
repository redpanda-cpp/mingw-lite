#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    PVOID __stdcall
    fallback_AddVectoredExceptionHandler(ULONG First,
                                         PVECTORED_EXCEPTION_HANDLER Handler);
  }
} // namespace mingw_thunk
