#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DLL_DIRECTORY_COOKIE __stdcall
    fallback_AddDllDirectory(_In_ PCWSTR NewDirectory);
  }
} // namespace mingw_thunk
