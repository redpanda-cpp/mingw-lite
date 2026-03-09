#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_SetEnvironmentVariableW(_In_ LPCWSTR lpName,
                                                 _In_opt_ LPCWSTR lpValue);
  } // namespace f
} // namespace mingw_thunk
