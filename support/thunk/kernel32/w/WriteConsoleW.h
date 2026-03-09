#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_WriteConsoleW(_In_ HANDLE hConsoleOutput,
                                       _In_ const VOID *lpBuffer,
                                       _In_ DWORD nNumberOfCharsToWrite,
                                       _Out_opt_ LPDWORD lpNumberOfCharsWritten,
                                       _Reserved_ LPVOID lpReserved);
  } // namespace f
} // namespace mingw_thunk
