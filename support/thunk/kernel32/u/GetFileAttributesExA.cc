#include <thunk/_common.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetFileAttributesExA,
                 _In_ LPCSTR lpFileName,
                 _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                 _Out_ LPVOID lpFileInformation)
  {
    stl::wstring u_file_name = internal::u2w(lpFileName);
    return GetFileAttributesExW(
        u_file_name.c_str(), fInfoLevelId, lpFileInformation);
  }
} // namespace mingw_thunk
