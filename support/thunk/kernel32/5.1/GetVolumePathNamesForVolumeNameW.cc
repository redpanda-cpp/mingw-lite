#include "GetVolumePathNamesForVolumeNameW.h"

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
    __DISPATCH_THUNK_2(GetVolumePathNamesForVolumeNameW,
                       const auto pfn =
                           try_get_GetVolumePathNamesForVolumeNameW(),
                       pfn,
                       &f::fallback_GetVolumePathNamesForVolumeNameW);

    return dllimport_GetVolumePathNamesForVolumeNameW(lpszVolumeName,
                                                      lpszVolumePathNames,
                                                      cchBufferLength,
                                                      lpcchReturnLength);
  }

  namespace f
  {
    BOOL __stdcall
    fallback_GetVolumePathNamesForVolumeNameW(_In_ LPCWSTR lpszVolumeName,
                                              _Out_ LPWCH lpszVolumePathNames,
                                              _In_ DWORD cchBufferLength,
                                              _Out_ PDWORD lpcchReturnLength)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return FALSE;
    }
  } // namespace f
} // namespace mingw_thunk
