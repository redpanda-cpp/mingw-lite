#include "LockFileEx.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
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
    __DISPATCH_THUNK_2(
        LockFileEx, i::is_nt(), &__ms_LockFileEx, &f::win9x_LockFileEx);

    return dllimport_LockFileEx(hFile,
                                dwFlags,
                                dwReserved,
                                nNumberOfBytesToLockLow,
                                nNumberOfBytesToLockHigh,
                                lpOverlapped);
  }

  namespace f
  {
    BOOL __stdcall win9x_LockFileEx(_In_ HANDLE hFile,
                                    _In_ DWORD dwFlags,
                                    DWORD dwReserved,
                                    _In_ DWORD nNumberOfBytesToLockLow,
                                    _In_ DWORD nNumberOfBytesToLockHigh,
                                    _Inout_ LPOVERLAPPED lpOverlapped)
    {
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
  } // namespace f
} // namespace mingw_thunk
