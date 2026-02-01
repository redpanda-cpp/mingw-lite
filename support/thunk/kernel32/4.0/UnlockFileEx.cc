#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 20,
                 BOOL,
                 WINAPI,
                 UnlockFileEx,
                 _In_ HANDLE hFile,
                 DWORD dwReserved,
                 _In_ DWORD nNumberOfBytesToLockLow,
                 _In_ DWORD nNumberOfBytesToLockHigh,
                 _Inout_ LPOVERLAPPED lpOverlapped)
  {
    if (internal::is_nt())
      return __ms_UnlockFileEx(hFile,
                               dwReserved,
                               nNumberOfBytesToLockLow,
                               nNumberOfBytesToLockHigh,
                               lpOverlapped);

    return UnlockFile(
        hFile, 0, 0, nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh);
  }
} // namespace mingw_thunk
