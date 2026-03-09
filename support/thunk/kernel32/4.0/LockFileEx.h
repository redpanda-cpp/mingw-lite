#pragma once

#include <windows.h>

namespace mingw_thunk
{
  namespace f
  {
    BOOL __stdcall win9x_LockFileEx(_In_ HANDLE hFile,
                                    _In_ DWORD dwFlags,
                                    DWORD dwReserved,
                                    _In_ DWORD nNumberOfBytesToLockLow,
                                    _In_ DWORD nNumberOfBytesToLockHigh,
                                    _Inout_ LPOVERLAPPED lpOverlapped);
  } // namespace f
} // namespace mingw_thunk
