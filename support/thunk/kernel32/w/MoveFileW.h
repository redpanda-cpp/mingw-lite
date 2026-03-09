#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_MoveFileW(_In_ LPCWSTR lpExistingFileName,
                                   _In_ LPCWSTR lpNewFileName);
  } // namespace f
} // namespace mingw_thunk
