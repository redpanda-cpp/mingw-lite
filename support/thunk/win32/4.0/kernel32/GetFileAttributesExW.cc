#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errhandlingapi.h>
#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetFileAttributesExW,
                 _In_ LPCWSTR lpFileName,
                 _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                 _Out_ LPVOID lpFileInformation)
  {
    if (internal::is_nt())
      return get_GetFileAttributesExW()(lpFileName, fInfoLevelId, lpFileInformation);

    auto aname = internal::narrow(lpFileName);
    return GetFileAttributesExA(aname.c_str(), fInfoLevelId, lpFileInformation);
  }
} // namespace mingw_thunk
