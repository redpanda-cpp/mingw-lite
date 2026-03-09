#include "UnlockFileEx.h"

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
    __DISPATCH_THUNK_2(
        UnlockFileEx, i::is_nt(), &__ms_UnlockFileEx, &f::win9x_UnlockFileEx);

    return dllimport_UnlockFileEx(hFile,
                                  dwReserved,
                                  nNumberOfBytesToLockLow,
                                  nNumberOfBytesToLockHigh,
                                  lpOverlapped);
  }

  namespace f
  {
    BOOL __stdcall win9x_UnlockFileEx(_In_ HANDLE hFile,
                                      DWORD dwReserved,
                                      _In_ DWORD nNumberOfBytesToLockLow,
                                      _In_ DWORD nNumberOfBytesToLockHigh,
                                      _Inout_ LPOVERLAPPED lpOverlapped)
    {
      return UnlockFile(
          hFile, 0, 0, nNumberOfBytesToLockLow, nNumberOfBytesToLockHigh);
    }
  } // namespace f
} // namespace mingw_thunk
