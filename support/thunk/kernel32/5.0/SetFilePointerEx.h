#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL WINAPI
    fallback_SetFilePointerEx(_In_ HANDLE hFile,
                              _In_ LARGE_INTEGER liDistanceToMove,
                              _Out_opt_ PLARGE_INTEGER lpNewFilePointer,
                              _In_ DWORD dwMoveMethod);
  } // namespace f
} // namespace mingw_thunk
