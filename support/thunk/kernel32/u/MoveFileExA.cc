#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 MoveFileExA,
                 _In_ LPCSTR lpExistingFileName,
                 _In_opt_ LPCSTR lpNewFileName,
                 _In_ DWORD dwFlags)
  {
    if (!lpExistingFileName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::w_str w_exist;
    if (!w_exist.from_u(lpExistingFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    d::w_str w_new;
    if (lpNewFileName && !w_new.from_u(lpNewFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    return MoveFileExW(
        w_exist.c_str(), lpNewFileName ? w_new.c_str() : nullptr, dwFlags);
  }
} // namespace mingw_thunk
