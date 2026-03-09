#include "GetSystemWow64DirectoryA.h"

#include <thunk/_common.h>

#include <windows.h>

namespace mingw_thunk
{
  __DEFINE_THUNK(kernel32,
                 8,
                 UINT,
                 WINAPI,
                 GetSystemWow64DirectoryA,
                 _Out_ LPSTR lpBuffer,
                 _In_ UINT uSize)
  {
    __DISPATCH_THUNK_2(GetSystemWow64DirectoryA,
                       const auto pfn = try_get_GetSystemWow64DirectoryA(),
                       pfn,
                       &f::win32_GetSystemWow64DirectoryA);

    return dllimport_GetSystemWow64DirectoryA(lpBuffer, uSize);
  }

  namespace f
  {
    UINT __stdcall win32_GetSystemWow64DirectoryA(_Out_ LPSTR lpBuffer,
                                                  _In_ UINT uSize)
    {
      SetLastError(ERROR_CALL_NOT_IMPLEMENTED);
      return 0;
    }
  } // namespace f
} // namespace mingw_thunk
