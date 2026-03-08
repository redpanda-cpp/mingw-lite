#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 24,
                 BOOL,
                 WINAPI,
                 CopyFileExA,
                 _In_ LPCSTR lpExistingFileName,
                 _In_ LPCSTR lpNewFileName,
                 _In_opt_ LPPROGRESS_ROUTINE lpProgressRoutine,
                 _In_opt_ LPVOID lpData,
                 _In_opt_ LPBOOL pbCancel,
                 _In_ DWORD dwCopyFlags)
  {
    if (!lpExistingFileName || !lpNewFileName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::w_str w_exist;
    if (!w_exist.from_u(lpExistingFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    d::w_str w_new;
    if (!w_new.from_u(lpNewFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    return CopyFileExW(w_exist.c_str(),
                       w_new.c_str(),
                       lpProgressRoutine,
                       lpData,
                       pbCancel,
                       dwCopyFlags);
  }
} // namespace mingw_thunk
