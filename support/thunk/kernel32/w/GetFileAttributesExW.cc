#include "GetFileAttributesExW.h"

#include <thunk/_common.h>
#include <thunk/_no_thunk.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <windows.h>

namespace mingw_thunk
{
  // Windows 98 (stub)
  // Windows NT 4.0
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 GetFileAttributesExW,
                 _In_ LPCWSTR lpFileName,
                 _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                 _Out_ LPVOID lpFileInformation)
  {
    if (internal::is_nt()) {
#if THUNK_LEVEL >= NTDDI_WIN98
      return __ms_GetFileAttributesExW(
          lpFileName, fInfoLevelId, lpFileInformation);
#else
      return get_GetFileAttributesExW()(
          lpFileName, fInfoLevelId, lpFileInformation);
#endif
    }

    return impl::win9x_GetFileAttributesExW(
        lpFileName, fInfoLevelId, lpFileInformation);
  }

  namespace impl
  {
    BOOL win9x_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                                    _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                    _Out_ LPVOID lpFileInformation)
    {
#if THUNK_LEVEL >= NTDDI_WIN98
      stl::string a_file_name = internal::w2a(lpFileName);
      return __ms_GetFileAttributesExA(
          a_file_name.c_str(), fInfoLevelId, lpFileInformation);
#else
      if (internal::os_geq(VER_PLATFORM_WIN32_WINDOWS, 4, 10)) {
        stl::string a_file_name = internal::w2a(lpFileName);
        return kernel32_GetFileAttributesExA()(
            a_file_name.c_str(), fInfoLevelId, lpFileInformation);
      }
      return win95_GetFileAttributesExW(
          lpFileName, fInfoLevelId, lpFileInformation);
#endif
    }

    BOOL win95_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                                    _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                                    _Out_ LPVOID lpFileInformation)
    {
      if (fInfoLevelId != GetFileExInfoStandard) {
        SetLastError(ERROR_NOT_SUPPORTED);
        return FALSE;
      }

      DWORD attr = GetFileAttributesW(lpFileName);
      if (attr == INVALID_FILE_ATTRIBUTES)
        return FALSE;

      if (attr & FILE_ATTRIBUTE_DIRECTORY) {
        // Windows 9x: `CreateFileW(-A)` cannot open directories,
        // even if `FILE_FLAG_BACKUP_SEMANTICS` is specified.

        auto *info =
            reinterpret_cast<WIN32_FILE_ATTRIBUTE_DATA *>(lpFileInformation);
        info->dwFileAttributes = attr;
        info->ftCreationTime = {};
        info->ftLastAccessTime = {};
        info->ftLastWriteTime = {};
        info->nFileSizeHigh = 0;
        info->nFileSizeLow = 0;

        return TRUE;
      } else {
        HANDLE hFile =
            CreateFileW(lpFileName,
                        GENERIC_READ,
                        // Windows 9x: `FILE_SHARE_DELETE` not supported
                        FILE_SHARE_READ | FILE_SHARE_WRITE,
                        nullptr,
                        OPEN_EXISTING,
                        FILE_FLAG_BACKUP_SEMANTICS,
                        nullptr);
        if (hFile == INVALID_HANDLE_VALUE)
          return FALSE;

        FILETIME atime, mtime, ctime;
        if (!GetFileTime(hFile, &ctime, &atime, &mtime)) {
          CloseHandle(hFile);
          return FALSE;
        }

        DWORD low, high;
        low = GetFileSize(hFile, &high);
        if (low == INVALID_FILE_SIZE && GetLastError()) {
          CloseHandle(hFile);
          return FALSE;
        }

        CloseHandle(hFile);

        auto *info =
            reinterpret_cast<WIN32_FILE_ATTRIBUTE_DATA *>(lpFileInformation);
        info->dwFileAttributes = attr;
        info->ftCreationTime = ctime;
        info->ftLastAccessTime = atime;
        info->ftLastWriteTime = mtime;
        info->nFileSizeHigh = high;
        info->nFileSizeLow = low;

        return TRUE;
      }
    }
  } // namespace impl
} // namespace mingw_thunk
