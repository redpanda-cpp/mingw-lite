#include <thunk/_common.h>

#include <nocrt/ctype.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetVolumePathNameW,
                 _In_ LPCWSTR lpszFileName,
                 _Out_ LPWSTR lpszVolumePathName,
                 _In_ DWORD cchBufferLength)
  {
    if (const auto pfn = try_get_GetVolumePathNameW())
      return pfn(lpszFileName, lpszVolumePathName, cchBufferLength);

    if (cchBufferLength < 4) {
      SetLastError(ERROR_INSUFFICIENT_BUFFER);
      return FALSE;
    }

    // If you specify a relative directory or file name without a volume
    // qualifier, GetVolumePathName returns the drive letter of the boot volume.
    wchar_t system_directory[MAX_PATH];
    if (!libc::isalpha(lpszFileName[0]) || lpszFileName[1] != L':' ||
        lpszFileName[2] != L'\\') {
      GetSystemDirectoryW(system_directory, MAX_PATH);
      lpszFileName = system_directory;
    }

    lpszVolumePathName[0] = lpszFileName[0];
    lpszVolumePathName[1] = L':';
    lpszVolumePathName[2] = L'\\';
    lpszVolumePathName[3] = 0;

    return TRUE;
  }
} // namespace mingw_thunk
