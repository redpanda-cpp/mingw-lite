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
                 8,
                 BOOL,
                 WINAPI,
                 CreateDirectoryW,
                 _In_ LPCWSTR lpPathName,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes)
  {
    if (internal::is_nt())
      return __ms_CreateDirectoryW(lpPathName, lpSecurityAttributes);

    auto a_path_name = internal::w2a(lpPathName);
    return __ms_CreateDirectoryA(a_path_name.c_str(), lpSecurityAttributes);
  }
} // namespace mingw_thunk
