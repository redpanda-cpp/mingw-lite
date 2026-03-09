#include "GetSystemWow64DirectoryW.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 UINT,
                 WINAPI,
                 GetSystemWow64DirectoryW,
                 _Out_ LPWSTR lpBuffer,
                 _In_ UINT uSize)
  {
    __DISPATCH_THUNK_2(GetSystemWow64DirectoryW,
                       const auto pfn = try_get_GetSystemWow64DirectoryW(),
                       pfn,
                       &f::fallback_GetSystemWow64DirectoryW);

    return dllimport_GetSystemWow64DirectoryW(lpBuffer, uSize);
  }

  namespace f
  {
    UINT __stdcall fallback_GetSystemWow64DirectoryW(_Out_ LPWSTR lpBuffer,
                                                     _In_ UINT uSize)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
