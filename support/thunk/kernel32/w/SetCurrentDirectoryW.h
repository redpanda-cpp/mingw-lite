#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_SetCurrentDirectoryW(_In_ LPCWSTR lpPathName);
  } // namespace f
} // namespace mingw_thunk
