#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 16,
                 BOOL,
                 WINAPI,
                 GetVolumePathNamesForVolumeNameW,
                 _In_ LPCWSTR lpszVolumeName,

                 _Out_ LPWCH lpszVolumePathNames,
                 _In_ DWORD cchBufferLength,
                 _Out_ PDWORD lpcchReturnLength)
  {
    if (const auto pfn = try_get_GetVolumePathNamesForVolumeNameW())
      return pfn(lpszVolumeName,
                 lpszVolumePathNames,
                 cchBufferLength,
                 lpcchReturnLength);

    SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
    return FALSE;
  }
} // namespace mingw_thunk
