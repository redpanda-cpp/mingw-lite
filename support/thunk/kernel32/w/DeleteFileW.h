#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_DeleteFileW(_In_ LPCWSTR lpFileName);
  }
} // namespace mingw_thunk
