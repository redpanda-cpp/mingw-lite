#include "FindFirstVolumeW.h"

#include <thunk/_common.h>
#include <thunk/findvolumedata.h>

#include <windows.h>

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
    __DISPATCH_THUNK_2(FindFirstVolumeW,
                       const auto pfn = try_get_FindFirstVolumeW(),
                       pfn,
                       &f::fallback_FindFirstVolumeW);

    return dllimport_FindFirstVolumeW(lpszVolumeName, cchBufferLength);
  }

  namespace f
  {
    HANDLE __stdcall fallback_FindFirstVolumeW(_Out_ LPWSTR lpszVolumeName,
                                               _In_ DWORD cchBufferLength)
    {
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
          HeapAlloc(GetProcessHeap(), 0, sizeof(internal::find_volume_data)));

      if (!data) {
        SetLastError(ERROR_OUTOFMEMORY);
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
  } // namespace f
} // namespace mingw_thunk
