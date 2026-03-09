#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_CopyFileW(_In_ LPCWSTR lpExistingFileName,
                                   _In_ LPCWSTR lpNewFileName,
                                   _In_ BOOL bFailIfExists);
  }
} // namespace mingw_thunk
