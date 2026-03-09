#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HMODULE __stdcall win9x_LoadLibraryW(_In_ LPCWSTR lpLibFileName);
  } // namespace f
} // namespace mingw_thunk
