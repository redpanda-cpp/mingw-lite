#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 BOOL,
                 WINAPI,
                 CreateDirectoryW,
                 _In_ LPCWSTR lpPathName,
                 _In_opt_ LPSECURITY_ATTRIBUTES lpSecurityAttributes)
  {
    if (internal::is_nt())
      return get_CreateDirectoryW()(lpPathName, lpSecurityAttributes);

    auto a_path_name = internal::narrow(lpPathName);
    return CreateDirectoryA(a_path_name.c_str(), lpSecurityAttributes);
  }
} // namespace mingw_thunk
