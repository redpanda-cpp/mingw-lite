#include <errhandlingapi.h>
#include <thunk/_common.h>

#include <windows.h>
#include <winerror.h>

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

    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
  }
} // namespace mingw_thunk
