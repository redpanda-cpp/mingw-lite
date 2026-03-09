#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    LPWCH __stdcall win9x_GetEnvironmentStringsW();
  }
} // namespace mingw_thunk
