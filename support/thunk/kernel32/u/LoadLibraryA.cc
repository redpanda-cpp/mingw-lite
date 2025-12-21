#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, HMODULE, WINAPI, LoadLibraryA, _In_ LPCSTR lpLibFileName)
  {
    stl::wstring w_name = internal::u2w(lpLibFileName);
    return LoadLibraryW(w_name.c_str());
  }
} // namespace mingw_thunk
