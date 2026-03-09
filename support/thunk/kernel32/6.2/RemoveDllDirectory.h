#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall
    fallback_RemoveDllDirectory(_In_ DLL_DIRECTORY_COOKIE Cookie);
  }
} // namespace mingw_thunk
