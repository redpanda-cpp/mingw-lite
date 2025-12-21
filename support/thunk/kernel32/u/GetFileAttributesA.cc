#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(
      kernel32, 4, DWORD, WINAPI, GetFileAttributesA, _In_ LPCSTR lpFileName)
  {
    stl::wstring w_file_name = internal::u2w(lpFileName);
    return GetFileAttributesW(w_file_name.c_str());
  }
} // namespace mingw_thunk
