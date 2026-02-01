#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace impl
  {
    BOOL win9x_WriteConsoleW(_In_ HANDLE hConsoleOutput,
                             _In_ const VOID *lpBuffer,
                             _In_ DWORD nNumberOfCharsToWrite,
                             _Out_opt_ LPDWORD lpNumberOfCharsWritten,
                             _Reserved_ LPVOID lpReserved);
  } // namespace impl
} // namespace mingw_thunk
