#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall fallback_CancelIoEx(_In_ HANDLE hFile,
                                       _In_opt_ LPOVERLAPPED lpOverlapped);
  }
} // namespace mingw_thunk
