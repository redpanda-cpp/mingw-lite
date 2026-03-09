#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    DWORD __stdcall win9x_GetFileAttributesW(_In_ LPCWSTR lpFileName);
  } // namespace f
} // namespace mingw_thunk