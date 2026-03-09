#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI fallback_GetFileSizeEx(_In_ HANDLE hFile,
                                       _Out_ PLARGE_INTEGER lpFileSize);
  } // namespace f
} // namespace mingw_thunk
