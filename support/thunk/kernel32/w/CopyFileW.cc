#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 95 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 CopyFileW,
                 _In_ LPCWSTR lpExistingFileName,
                 _In_ LPCWSTR lpNewFileName,
                 _In_ BOOL bFailIfExists)
  {
    if (internal::is_nt())
      return __ms_CopyFileW(lpExistingFileName, lpNewFileName, bFailIfExists);

    stl::string a_old_file = internal::w2a(lpExistingFileName);
    stl::string a_new_file = internal::w2a(lpNewFileName);
    return __ms_CopyFileA(
        a_old_file.c_str(), a_new_file.c_str(), bFailIfExists);
  }
} // namespace mingw_thunk
