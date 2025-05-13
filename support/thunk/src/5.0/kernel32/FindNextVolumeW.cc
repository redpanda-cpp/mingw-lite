#include <thunk/_common.h>
#include <thunk/findvolumedata.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 12,
                 BOOL,
                 WINAPI,
                 FindNextVolumeW,
                 _In_ HANDLE hFindVolume,
                 _Out_ LPWSTR lpszVolumeName,
                 _In_ DWORD cchBufferLength)
  {
    if (const auto pfn = try_get_FindNextVolumeW())
      return pfn(hFindVolume, lpszVolumeName, cchBufferLength);

    if (cchBufferLength < 4) {
      SetLastError(ERROR_INVALID_PARAMETER);
      return FALSE;
    }

    auto *data = reinterpret_cast<internal::find_volume_data *>(hFindVolume);

    DWORD volumes = data->volumes;
    DWORD drive = data->current + 1;
    for (; drive < 26; drive++) {
      if (volumes & (1 << drive)) {
        break;
      }
    }

    if (drive >= 26) {
      SetLastError(ERROR_NO_MORE_FILES);
      return FALSE;
    }

    data->current = drive;

    lpszVolumeName[0] = 'A' + drive;
    lpszVolumeName[1] = ':';
    lpszVolumeName[2] = '\\';
    lpszVolumeName[3] = '\0';

    return TRUE;
  }
} // namespace mingw_thunk
