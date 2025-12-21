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
    stl::wstring w_exist = internal::u2w(lpExistingFileName);
    stl::wstring w_new;
    if (lpNewFileName)
      w_new = internal::u2w(lpNewFileName);
    return MoveFileExW(
        w_exist.c_str(), lpNewFileName ? w_new.c_str() : nullptr, dwFlags);
  }
} // namespace mingw_thunk
