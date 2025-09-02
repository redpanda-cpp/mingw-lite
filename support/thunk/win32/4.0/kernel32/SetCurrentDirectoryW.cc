#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, BOOL, WINAPI, SetCurrentDirectoryW, _In_ LPCWSTR lpPathName)
  {
    if (internal::is_nt())
      return get_SetCurrentDirectoryW()(lpPathName);

    stl::string a_path_name = internal::narrow(lpPathName);
    return SetCurrentDirectoryA(a_path_name.c_str());
  }
} // namespace mingw_thunk
