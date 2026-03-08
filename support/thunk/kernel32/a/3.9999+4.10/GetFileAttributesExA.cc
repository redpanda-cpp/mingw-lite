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
    if (const auto pfn = try_get_GetFileAttributesExA())
      return pfn(lpFileName, fInfoLevelId, lpFileInformation);

    if (!lpFileName) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    d::w_str w_file_name;
    if (!w_file_name.from_a(lpFileName)) {
      SetLastError(ERROR_OUTOFMEMORY);
      return FALSE;
    }

    return GetFileAttributesExW(
        w_file_name.c_str(), fInfoLevelId, lpFileInformation);
  }
} // namespace mingw_thunk
