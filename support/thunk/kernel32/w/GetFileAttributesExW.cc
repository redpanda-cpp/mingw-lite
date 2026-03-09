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
#if THUNK_LEVEL >= NTDDI_WIN98
    __DISPATCH_THUNK_2(GetFileAttributesExW,
                       i::is_nt(),
                       &__ms_GetFileAttributesExW,
                       &f::win98_GetFileAttributesExW);
#else
    __DISPATCH_THUNK_3(GetFileAttributesExW,
                       i::is_nt(),
                       get_GetFileAttributesExW(),
                       i::os_version() >= g::win32_win98,
                       &f::win98_GetFileAttributesExW,
                       &f::win95_GetFileAttributesExW);
#endif

    return dllimport_GetFileAttributesExW(
        lpFileName, fInfoLevelId, lpFileInformation);
  }

  namespace f
  {
    BOOL __stdcall
    win98_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                               _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                               _Out_ LPVOID lpFileInformation)
    {
      if (!lpFileName) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

      d::a_str a_file_name;
      if (!a_file_name.from_w(lpFileName))
        return FALSE;

#if THUNK_LEVEL >= NTDDI_WIN98
      static const auto GetFileAttributesExA = __ms_GetFileAttributesExA;
#else
      static const auto GetFileAttributesExA = kernel32_GetFileAttributesExA();
#endif

      return GetFileAttributesExA(
          a_file_name.c_str(), fInfoLevelId, lpFileInformation);
    }

    BOOL __stdcall
    win95_GetFileAttributesExW(_In_ LPCWSTR lpFileName,
                               _In_ GET_FILEEX_INFO_LEVELS fInfoLevelId,
                               _Out_ LPVOID lpFileInformation)
    {
      if (!lpFileName || !lpFileInformation) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
      }

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
            static_cast<WIN32_FILE_ATTRIBUTE_DATA *>(lpFileInformation);
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
  } // namespace f
} // namespace mingw_thunk
