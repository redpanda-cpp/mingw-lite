#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>
#include <winerror.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 MoveFileExW,
                 _In_ LPCWSTR lpExistingFileName,
                 _In_opt_ LPCWSTR lpNewFileName,
                 _In_ DWORD dwFlags)
  {
    if (internal::is_nt())
      return get_MoveFileExW()(lpExistingFileName, lpNewFileName, dwFlags);

    auto a_exist = internal::narrow(lpExistingFileName);
    stl::string a_new;
    if (lpNewFileName)
      a_new = internal::narrow(lpNewFileName);

    return MoveFileExA(
        a_exist.c_str(), lpNewFileName ? a_new.c_str() : nullptr, dwFlags);
  }
} // namespace mingw_thunk
