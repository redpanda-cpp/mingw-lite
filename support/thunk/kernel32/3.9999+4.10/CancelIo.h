#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_CancelIo(_In_ HANDLE hFile);
  }
} // namespace mingw_thunk
