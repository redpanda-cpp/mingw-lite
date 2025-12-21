#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, HMODULE, WINAPI, LoadLibraryW, _In_ LPCWSTR lpLibFileName)
  {
    if (internal::is_nt())
      return __ms_LoadLibraryW(lpLibFileName);

    stl::string a_name = internal::w2a(lpLibFileName);
    return __ms_LoadLibraryA(a_name.c_str());
  }
} // namespace mingw_thunk
