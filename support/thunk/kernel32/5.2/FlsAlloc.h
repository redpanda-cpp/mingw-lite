#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall
    fallback_FlsAlloc(_In_opt_ PFLS_CALLBACK_FUNCTION lpCallback);
  }
} // namespace mingw_thunk
