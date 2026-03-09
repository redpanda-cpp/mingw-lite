#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_FreeEnvironmentStringsW(LPWCH penv);
  }
} // namespace mingw_thunk
