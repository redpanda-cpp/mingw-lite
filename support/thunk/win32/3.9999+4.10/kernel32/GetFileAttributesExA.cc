#include <handleapi.h>
#include <thunk/_common.h>
#include <thunk/os.h>
#include <thunk/string.h>

#include <errhandlingapi.h>
#include <windows.h>
#include <winerror.h>

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

    if (fInfoLevelId != GetFileExInfoStandard) {
      SetLastError(ERROR_NOT_SUPPORTED);
      return FALSE;
    }

    DWORD attr = GetFileAttributesA(lpFileName);
    if (attr == INVALID_FILE_ATTRIBUTES)
      return FALSE;

    HANDLE hFile = CreateFileA(lpFileName,
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
} // namespace mingw_thunk
