#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    HWND WINAPI fallback_GetConsoleWindow();
  } // namespace f
} // namespace mingw_thunk
