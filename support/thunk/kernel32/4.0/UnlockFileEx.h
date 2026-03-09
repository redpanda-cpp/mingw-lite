#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_UnlockFileEx(_In_ HANDLE hFile,
                                      DWORD dwReserved,
                                      _In_ DWORD nNumberOfBytesToLockLow,
                                      _In_ DWORD nNumberOfBytesToLockHigh,
                                      _Inout_ LPOVERLAPPED lpOverlapped);
  } // namespace f
} // namespace mingw_thunk
