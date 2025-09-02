#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 24,
                 BOOL,
                 WINAPI,
                 LockFileEx,
                 _In_ HANDLE hFile,
                 _In_ DWORD dwFlags,
                 DWORD dwReserved,
                 _In_ DWORD nNumberOfBytesToLockLow,
                 _In_ DWORD nNumberOfBytesToLockHigh,
                 _Inout_ LPOVERLAPPED lpOverlapped)
  {
    if (internal::is_nt())
      return get_LockFileEx()(hFile,
                              dwFlags,
                              dwReserved,
                              nNumberOfBytesToLockLow,
                              nNumberOfBytesToLockHigh,
                              lpOverlapped);

    if (!(dwFlags & LOCKFILE_EXCLUSIVE_LOCK)) {
      // shared lock is not supported
      SetLastError(ERROR_NOT_SUPPORTED);
      return FALSE;
    }

    if (dwFlags & LOCKFILE_FAIL_IMMEDIATELY) {
      return LockFile(
          hFile, 0, 0, nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh);
    } else {
      while (true) {
        bool ok = LockFile(
            hFile, 0, 0, nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh);
        if (!ok && GetLastError() == ERROR_LOCK_VIOLATION)
          Sleep(0);
        else
          return ok;
      }
    }
  }
} // namespace mingw_thunk
