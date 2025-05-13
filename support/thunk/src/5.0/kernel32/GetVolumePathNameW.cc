#include <thunk/_common.h>
#include <thunk/libc/ctype.h>

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

    if (!internal::isalpha(lpszFileName[0]) || lpszFileName[1] != L':' ||
        lpszFileName[2] != L'\\') {
      wchar_t system_directory[MAX_PATH];
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
