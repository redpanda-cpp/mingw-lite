#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    LPWSTR *__stdcall win9x_CommandLineToArgvW(_In_ LPCWSTR lpCmdLine,
                                               _Out_ int *pNumArgs);
  }
} // namespace mingw_thunk
