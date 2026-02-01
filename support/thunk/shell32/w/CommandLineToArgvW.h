#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    LPWSTR *win9x_CommandLineToArgvW(_In_ LPCWSTR lpCmdLine,
                                     _Out_ int *pNumArgs);
  }
} // namespace mingw_thunk
