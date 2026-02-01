#include <thunk/_common.h>
#include <thunk/findvolumedata.h>

#include <windows.h>

#include <nocrt/stdlib.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 HANDLE,
                 WINAPI,
                 FindFirstVolumeW,
                 _Out_ LPWSTR lpszVolumeName,
                 _In_ DWORD cchBufferLength)
  {
    if (const auto pfn = try_get_FindFirstVolumeW())
      return pfn(lpszVolumeName, cchBufferLength);

    if (cchBufferLength < 4) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return INVALID_HANDLE_VALUE;
    }

    DWORD volumes = GetLogicalDrives();
    if (volumes == 0) {
      return INVALID_HANDLE_VALUE;
    }

    DWORD drive = 0;
    for (; drive < 26; drive++) {
      if (volumes & (1 << drive)) {
        break;
      }
    }

    auto *data = static_cast<internal::find_volume_data *>(
        libc::malloc(sizeof(internal::find_volume_data)));

    if (!data) {
      SetLastError(ERROR_NOT_ENOUGH_MEMORY);
      return INVALID_HANDLE_VALUE;
    }

    data->volumes = volumes;
    data->current = drive;

    lpszVolumeName[0] = 'A' + drive;
    lpszVolumeName[1] = ':';
    lpszVolumeName[2] = '\\';
    lpszVolumeName[3] = '\0';

    return data;
  }
} // namespace mingw_thunk
