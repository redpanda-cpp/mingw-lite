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
                 CopyFileExW,
                 _In_ LPCWSTR lpExistingFileName,
                 _In_ LPCWSTR lpNewFileName,
                 _In_opt_ LPPROGRESS_ROUTINE lpProgressRoutine,
                 _In_opt_ LPVOID lpData,
                 _In_opt_ LPBOOL pbCancel,
                 _In_ DWORD dwCopyFlags)
  {
    if (internal::is_nt())
      return get_CopyFileExW()(lpExistingFileName,
                               lpNewFileName,
                               lpProgressRoutine,
                               lpData,
                               pbCancel,
                               dwCopyFlags);

    // Windows 9x: `CopyFileExW(-A)` is a stub.
    bool fail_if_exists = dwCopyFlags & COPY_FILE_FAIL_IF_EXISTS;
    return CopyFileW(lpExistingFileName, lpNewFileName, fail_if_exists);
  }
} // namespace mingw_thunk
